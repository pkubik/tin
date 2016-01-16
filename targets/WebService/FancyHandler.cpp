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

    response.code = 200;
    response.headers["Content-Type"] = "text/html";

    NL::Template::LoaderFile loader; // Let's use the default loader that loads files from disk.

    NL::Template::Template t( loader );

    t.load( "res/templates/main.html" );               // Load & parse the main template and its dependencies.
    t.block( "table" ).repeat( 2 );     // We need to know in advance that the "items" block will repeat 3 times.

    std::string sql = "Select table_name from information_schema.tables where table_schema='public'";
    std::ostringstream msgStream;
/* Create a non-transactional object. */
	pqxx::connection conn("user=tin host=czadzik24.pl port=5432 password=haslo dbname=tin");
	pqxx::nontransaction N(conn);

/* Execute SQL query */
	pqxx::result R( N.exec( sql ));
	int i=0;
	for (pqxx::result::const_iterator c=R.begin(); c!=R.end();++c) {
		t.block( "table" )[ i ].set("name", c[0].as<std::string>());
		++i;
	}
	conn.disconnect ();
    t.render( msgStream ); // Render the template with the variables we've set above
    response.message=msgStream.str();
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
