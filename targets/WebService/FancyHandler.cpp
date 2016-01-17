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
    : configuration()
    , dataBase("")
{
    configuration.setConfiguration();
    dataBase.setConnectionString(configuration.getConnctionString());
}

Response FancyHandler::handle(const Request& request, RequestError error) {
	LOGT("Echo handler handles request...");

    if (error != RequestError::NONE) {
        return handleRequestError();
    }

    if (request.getMethod() != Request::GET || request.getVersion() != "HTTP/1.1") {
        return handleGeneralError(request);
    }
    
    // Obsluga prostego echa
    std::string echo = "/echo/";
    if (request.getResource().compare(0, echo.length(), echo) == 0) {
        return handleSuccessEcho(request);
    }
    
    /*
     * W pierwszej kolejnosci sprawdzamy czy uzytkownik wszedl w 'Strone glowna' czli /
     * czy moze zapytanie ma parametry.
     * Sprawdzenie czy w pliku config.ini podano tabele startowa.
     * Jeśli nie podano to wywolujemy handleSuccessMain ktory wypisuje liste dostepnych tabel
     * 
     * Jesli podano to przechodzimy do wyswietlenia poczatkowej zawartosci konkretnej tabeli.
     * 
     */ 
    
     std::string main = "/";
     if (request.getResource().compare(0, echo.length(), main) == 0) { // czy '/'
        if(configuration.getStartingTable() == "") //co przewiduje config.ini
        {
            return handleSuccessMain(request); // startingTabel puste
        }
        else
        {
            std::string table = configuration.getStartingTable();
            return handleSuccessTable(request, table); // pokazanie startingTable
        }
     }
     else // przypadek z parametrami
     {
         
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

// to do!
Response FancyHandler::handleSuccessTable(const Request& request, const std::string tableName) const {
    Response response;
    std::ostringstream msgStream;

    NL::Template::LoaderFile loader;

    NL::Template::Template t( loader );
   
    t.load( "res/templates/table.html" );
    t.set("title", "Tabela \"" + tableName + "\"");
    t.set("head_title", "Tabela \"" + tableName + "\"");
    std::string sql = "select * from " + tableName + ";";
    Table result = dataBase.execQuery(sql);

    int size = result.tableSize();
    int col_num = result.rowSize();

    t.block( "row" ).repeat( size );
    t.block( "header_col" ).repeat( col_num );
    /* fill header row fields */
    for (int j=0; j < col_num;++j) {
        t.block("header_col")[j].set("field", result.getColumnsNames()[j]);
    }

    NL::Template::Block & block = t.block( "row" );
    /* fill remaining rows */
    for (int i=0;i<size;++i) {

        block[i].block( "col" ).repeat( col_num );
        /* fill columns in a specific row */
        for (int j=0; j < col_num;++j) {
            block[i].block("col")[j].set("field", result.getRow(i)[j]);

        }
    }
    t.render( msgStream );

    response.message=msgStream.str();
    response.code = 200;
    response.headers["Content-Type"] = "text/html";

    return response;
}

Response FancyHandler::handleSuccessMain(const Request& request) const {
    Response response;
    std::ostringstream msgStream;

    NL::Template::LoaderFile loader;

    NL::Template::Template t( loader );

    t.load( "res/templates/main.html" );
    t.set("res_dir", "/home/patryk/git/tin/build");
    t.set("title", "Lista wszystkich tabel");
    t.set("head_title", "Lista wszystkich tabel");
    std::string sql = "select distinct "
    		"table_name, "
    		"count(*) over (partition by table_name) as column_count "
    		"from information_schema.columns "
    			"where table_schema='public' "
    			"order by table_name asc;";
    Table result = dataBase.execQuery(sql);

    int size = result.tableSize();
    int col_num = result.rowSize();

    t.block( "row" ).repeat( size );
    t.block( "header_col" ).repeat( col_num );
    /* fill header row fields */
	for (int j=0; j < col_num;++j) {
		t.block("header_col")[j].set("field", result.getColumnsNames()[j]);
	}

    NL::Template::Block & block = t.block( "row" );
    /* fill remaining rows */
	for (int i=0;i<size;++i) {

		block[i].block( "col" ).repeat( col_num );
		/* fill columns in a specific row */
		for (int j=0; j < col_num;++j) {
			block[i].block("col")[j].set("field", result.getRow(i)[j]);

		}
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
