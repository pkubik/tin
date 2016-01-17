/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#include "FancyHandler.hpp"
#include <sstream>

using namespace server;
using namespace table;

FancyHandler::FancyHandler()
    : dataBase("")
{

}

Response FancyHandler::handle(const Request& request, RequestError error) {
	LOGT("Echo handler handles request...");

    if (error != RequestError::NONE) {
        return handleRequestError();
    }

    if (request.getMethod() != Request::GET || request.getVersion() != "HTTP/1.1") {
        return handleGeneralError(request);
    }

    std::string echo = "/echo/";
    if (request.getResource().compare(0, echo.length(), echo) == 0) {
        return handleSuccessEcho(request);
    }

   	std::string main = "/";
    if (request.getResource().compare(0, echo.length(), main) == 0) {
        return handleSuccessMain(request);
    }

    std::string student = "/student";
    if (request.getResource().compare(0, student.length(), student) == 0) {
        auto it = request.getParameters().find("q");
        if(request.getParameters().end() != it)
        {
            it = request.getParameters().find("id");
            if(request.getParameters().end() != it)
            {
                // wyswietla szczegoly studenta
            }
            it = request.getParameters().find("from");
            if(request.getParameters().end() != it)
            {
                it = request.getParameters().find("to");
                if(request.getParameters().end() != it)
                {
                     // wyswietl przedział from to
                }
            }
            Table tabela = dataBase.execQuery("SELECT * FROM students;");
            std::string mess = "\nLiczba kolumn: " + std::to_string(tabela.rowSize()) + "\nLiczba wierszy: " + std::to_string(tabela.tableSize());
            for (std::string str : tabela.getColumnsNames())
            {
                mess += " " + str;
            }
            for (std::string str : tabela.getColumnsTypes())
            {
                mess += " " + str;
            }
            std::cout << "\nOM OM OM\n";
            return handleSuccessTable(request, mess);

        }

        return handle404Error(request);
    }

    return handle404Error(request);
}

// internal handlers not required by the server API
server::Response FancyHandler::handleRequestError() const {
    Response response;

    response.code = 400;
    response.headers["Content-Type"] = "text/plain";
    response.message = "Invalid request.";;

    return response;
}

Response FancyHandler::handleGeneralError(const Request& request) const {
    Response response;

    response.code = 400;

    constexpr auto message = "Invalid request.";
    fillSimpleResponse(response, request, message);

    return response;
}

Response FancyHandler::handle404Error(const Request& request) const {
    Response response;

    response.code = 404;

    constexpr auto message = "Requested resource not found.";
    fillSimpleResponse(response, request, message);

    return response;
}

Response FancyHandler::handleSuccessEcho(const Request& request) const {
    Response response;

    response.code = 200;
    constexpr char echo[] = "/echo/";

    fillSimpleResponse(response, request, request.getResource().substr(sizeof(echo) - 1));

    return response;
}

Response FancyHandler::handleSuccessTable(const Request& request, std::string mess) const {
    Response response;

    response.code = 200;
    constexpr char echo[] = "/echo/";

    fillSimpleResponse(response, request, mess);

    return response;
}

Response FancyHandler::handleSuccessMain(const Request& request) const {
    Response response;
    std::ostringstream msgStream;

    NL::Template::LoaderFile loader;

    NL::Template::Template t( loader );

    t.load( "res/templates/main.html" );
    t.set("title", "Lista wszystkich tabel");

    std::string sql = "Select table_name, table_schema from information_schema.tables";
    Table result = dataBase.execQuery(sql);

    int size = result.tableSize();
    t.block( "table" ).repeat( size );
	for (int i=0;i<size;++i) {
		//NL::Template::Block & block = t.block( "table" )[i];
		t.block( "table" )[i].set("name", result.getRow(i)[0]);
		t.block( "table" )[i].set("schema", result.getRow(i)[1]);
	}
    t.render( msgStream );

    response.message=msgStream.str();
    response.code = 200;
    response.headers["Content-Type"] = "text/html";

    return response;
}

/*
* HTML templater call.
* Note: original templater API should be different! (accept additional parameters)
*/
std::string FancyHandler::generateHtmlTemplate(const std::string& message) const {
    auto ret = "<html>"
               "<head>"
               "<title>" + message + "</title>"
               "<body>"
               "<h1>Echo: " + message + "</h1>"
               "</body>"
               "</html>";

    return ret;
}

/*
* Handles simple echo response.
*/
void FancyHandler::fillSimpleResponse(Response& response,
                        const Request& request,
                        const std::string& message) const {
    if (acceptsHtml(request)) {
        response.headers["Content-Type"] = "text/html";
        response.message = generateHtmlTemplate(message);
    } else {
        response.headers["Content-Type"] = "text/plain";
        response.message = message;
    }
}

/*
 * Checks if the client accepts HTML response.
 */
bool FancyHandler::acceptsHtml(const Request& request) const {
    auto it = request.getHeaders().find("Content-Type");
    if (it == request.getHeaders().end() ||
        it->second.find("text/html") != std::string::npos) {
        return true;
    }

    return false;
}

store::DataStore& FancyHandler::getDataBase()
{
    return dataBase;
}
