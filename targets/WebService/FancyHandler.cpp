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
#include <fstream>

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
    std::string res = "/res/";
	if (request.getResource().compare(0, res.length(), res) == 0) {
		if (request.getResource().find("..") != std::string::npos)
			return handle404Error(request);
		else
			return handleFetchResource(request, request.getResource().substr(5));
	}

    // Obsluga prostego echa
    std::string echo = "/echo/";
    if (request.getResource().compare(0, echo.length(), echo) == 0) {
        return handleSuccessEcho(request);
    }
    
    std::string alltables = "/alltables";
    if (request.getResource().compare(0, alltables.length(), alltables) == 0) {
        return handleSuccessMain(request); // startingTabel puste
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
        auto itTable = request.getParameters().find("table");
        if(request.getParameters().end() != itTable) // czy jest parametr table
        {
            auto fkVector = dataBase.getPrimaryKeyColumnName(itTable->second);
            auto nameFk = fkVector.front();
            auto itFk = request.getParameters().find(nameFk);
            if(request.getParameters().end() != itFk) // czy jest parametr klucza obcego
            {
                return handleSuccessDetails(request, itTable->second); // pokazanie widoku detlicznego
            }
            else
            {
                return handleSuccessTable(request, itTable->second); // pokazanie tabeli
            }
            
        }
        else
        {
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

Response FancyHandler::handleFetchResource(const Request& request, const std::string& path) const {
    Response response;

    std::ifstream in(configuration.getRootResDir()+path);
    std::string msg((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    response.message=msg;
    response.code = 200;
    response.headers["Content-Type"] = "text/css";

    return response;
}


Response FancyHandler::handleSuccessDetails(const Request& request, const std::string tableName) const {
    Response response;
    std::ostringstream msgStream;
    NL::Template::LoaderFile loader;
    NL::Template::Template t( loader );
    t.load( configuration.getRootResDir()+"templates/detail.html" );

    t.render( msgStream );
    t.set("rootResDir", configuration.getRootResDir());
    t.set("head_title", "Widok detaliczny na tabeli \"" + tableName + "\"");
    t.set("table_path", "/?table="+tableName);
    t.set("table_description", "Cała tabela "+tableName);

    std::string columnName = *(dataBase.getPrimaryKeyColumnName(tableName).begin());
    auto it = request.getParameters().find(columnName);
    std::string columnValue = it->second;
    std::string sql = "select * from " + tableName + " where " + columnName + " = \'" + columnValue + "\';";
    Table result = dataBase.execQuery(sql);

    int col_num = result.rowSize();

    t.block( "row" ).repeat(col_num);
    for (int i=0;i<col_num;++i) {
    	t.block("row")[i].set("colName", result.getColumnsNames()[i]);
    	t.block("row")[i].set("colValue", result.getRow(0)[i]);
    }
    t.render( msgStream );

    response.message=msgStream.str();
    response.code = 200;
    response.headers["Content-Type"] = "text/html";
    return response;
}


Response FancyHandler::handleSuccessTable(const Request& request, const std::string tableName) const {
    Response response;
    std::ostringstream msgStream;

    NL::Template::LoaderFile loader;

    NL::Template::Template t( loader );
   
    t.load( configuration.getRootResDir()+"templates/table.html" );
    t.set("rootResDir", configuration.getRootResDir());
    t.set("title", "Tabela \"" + tableName + "\"");
    t.set("head_title", "Tabela \"" + tableName + "\"");
    t.set("table_path", "/alltables");
    t.set("table_description", "Wszystkie tabele");
    std::string sql = "select * from " + tableName + ";";
    Table result = dataBase.execQuery(sql);

    int size = result.tableSize();
    int col_num = result.rowSize();
    
    std::tuple<bool,std::string,std::string> isFKcolumn[col_num];
    std::string pkColName = *(dataBase.getPrimaryKeyColumnName(tableName).begin());
    LOGT("pkColName: " + pkColName);
    int pkColNum=-1;
    for (int i=0;i<=col_num;++i) {
    	if (result.getColumnsNames()[i] == pkColName) {
    		pkColNum = i;
    		break;
    	}
    }
    t.block( "row" ).repeat( size );
    t.block( "header_col" ).repeat( col_num );
    /* fill header row fields */
    for (int j=0; j < col_num;++j) {
        
        const std::string colName = result.getColumnsNames()[j];
        isFKcolumn[j] = dataBase.relatedTable(tableName, colName);
        if(std::get<0>(isFKcolumn[j]))
        {
            std::string link = "<a href=/?table=" + std::get<1>(isFKcolumn[j]) + ">" + colName + "</a>";
            t.block("header_col")[j].set("field", link);
        }
        else
        {
            t.block("header_col")[j].set("field", colName);
        }
        
    }

    NL::Template::Block & block = t.block( "row" );
    /* fill remaining rows */
    for (int i=0;i<size;++i) {

        block[i].block( "col" ).repeat( col_num );

        /* fill columns in a specific row */
        for (int j=0; j < col_num;++j) {
            auto cell = result.getRow(i)[j];
            if(std::get<0>(isFKcolumn[j]))
            {
                std::string link = "<a href=/?table=" + std::get<1>(isFKcolumn[j]) + "&" + std::get<2>(isFKcolumn[j]) + "="+ cell +">" + cell + "</a>";
                block[i].block("col")[j].set("field", link);
            }
            else if (pkColNum == j) {
            	std::string link = "<a href=/?table=" + tableName + "&" + result.getColumnsNames()[j] + "="+ cell +">" + cell + "</a>";
            	block[i].block("col")[j].set("field", link);
            }
            else
            {
                block[i].block("col")[j].set("field", cell);
            }
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

    t.load( configuration.getRootResDir()+"templates/main.html" );
    t.set("rootResDir", configuration.getRootResDir());
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
