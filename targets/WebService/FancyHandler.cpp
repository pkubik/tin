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

    t.set("rootResDir", configuration.getRootResDir());
    t.set("head_title", "Widok detaliczny na tabeli \"" + tableName + "\"");
    t.set("title", tableName);
    t.set("alltable_path","/alltables?pgsize="+configuration.getPageSize()+"&pgnum=0");
    t.set("alltable_description", "All Tables");
    t.set("table_path", "/?table="+tableName+"&pgsize="+configuration.getPageSize()+"&pgnum=0");
    t.set("table_description", tableName);

	t.block("prev_btn").disable();
	t.block("next_btn").disable();

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
    t.set("title", tableName);
    t.set("head_title", "Tabela \"" + tableName + "\"");
    t.set("table_path", "/alltables?pgsize="+configuration.getPageSize()+"&pgnum=0");
    t.set("table_description", "All Tables");
    std::string sql = "select * from " + tableName;

    int pgSize = -1;
    int pgNum= -1;
    auto it_num = request.getParameters().find("pgnum");
    auto it_size = request.getParameters().find("pgsize");


	if (it_num != request.getParameters().end() and it_size != request.getParameters().end()) {
		pgSize = std::stoi(it_size->second);
		pgNum = std::stoi(it_num->second);
	}
	else {
		t.block("prev_btn").disable();
		t.block("next_btn").disable();
	}
	t.block("prev_btn").set("prev_url","/?table="+tableName+"&pgsize="+std::to_string(pgSize)+"&pgnum="+std::to_string(pgNum-1));
	t.block("next_btn").set("next_url","/?table="+tableName+"&pgsize="+std::to_string(pgSize)+"&pgnum="+std::to_string(pgNum+1));


	const std::vector<std::string> pkNames = dataBase.getPrimaryKeyColumnName(tableName);

    std::pair<bool,Table> resultPair = dataBase.execSelect(sql,pkNames, pgSize,pgNum);
    Table result = resultPair.second;
    bool isLastPage = resultPair.first;

    if (pgNum<=0 )
    	t.block("prev_btn").disable();
	if (isLastPage)
		t.block("next_btn").disable();

    int size = result.tableSize();
    int col_num = result.rowSize();
    
    std::tuple<bool,std::string,std::string> isFKcolumn[col_num];
    std::string pkColName = *(dataBase.getPrimaryKeyColumnName(tableName).begin());
    LOGT("pkColName: " + pkColName);
    int pkColNum=-1;
    for (int i=0;i<=col_num;++i) {
    	LOGT("pkColName search: " + result.getColumnsNames()[i]);
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
            std::string link = "<a href=/?table=" + std::get<1>(isFKcolumn[j])
            		+ "&pgsize="+configuration.getPageSize()+"&pgnum=0>" + colName + "</a>";
            t.block("header_col")[j].set("field", link);
        }
        else
        {
            t.block("header_col")[j].set("field", colName);
        }
        
    }
    if (size != 0) {
    	NL::Template::Block & block = t.block( "row" );
		/* fill remaining rows */
		for (int i=0;i<size;++i) {

			block[i].block( "col" ).repeat( col_num );

			/* fill columns in a specific row */
			for (int j=0; j < col_num;++j) {
				auto cell = result.getRow(i)[j];
				if(std::get<0>(isFKcolumn[j]))
				{
					std::string link = "<a href=/?table=" + std::get<1>(isFKcolumn[j]) + "&" + std::get<2>(isFKcolumn[j]) + "="+ cell
							+"&pgsize="+configuration.getPageSize()+"&pgnum=0>" + cell + "</a>";
					block[i].block("col")[j].set("field", link);
				}
				else if (pkColNum == j) {
					std::string link = "<a href=/?table=" + tableName + "&" + result.getColumnsNames()[j] + "="+ cell
							+"&pgsize="+configuration.getPageSize()+"&pgnum=0>" + cell + "</a>";
					block[i].block("col")[j].set("field", link);
				}
				else
				{
					block[i].block("col")[j].set("field", cell);
				}
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
    			"order by table_name asc";

    int pgSize = -1;
    int pgNum= -1;
    auto it_num = request.getParameters().find("pgnum");
    auto it_size = request.getParameters().find("pgsize");


	if (it_num != request.getParameters().end() and it_size != request.getParameters().end()) {
		pgSize = std::stoi(it_size->second);
		pgNum = std::stoi(it_num->second);
	}
	else {
		t.block("prev_btn").disable();
		t.block("next_btn").disable();
	}
	t.block("prev_btn").set("prev_url","/alltables?pgsize="+std::to_string(pgSize)+"&pgnum="+std::to_string(pgNum-1));
	t.block("next_btn").set("next_url","/alltables?pgsize="+std::to_string(pgSize)+"&pgnum="+std::to_string(pgNum+1));

	std::vector<string> orderByColNames = std::vector<string>();
	orderByColNames.push_back("table_name");

    std::pair<bool,Table> resultPair = dataBase.execSelect(sql,orderByColNames, pgSize,pgNum);
    Table result = resultPair.second;
    bool isLastPage = resultPair.first;

    if (pgNum<=0 )
    	t.block("prev_btn").disable();
	if (isLastPage)
		t.block("next_btn").disable();

    int size = result.tableSize();
    int col_num = result.rowSize();


    t.block( "row" ).repeat( size );
    t.block( "header_col" ).repeat( 2 );

    /* fill header row fields */
	t.block("header_col")[0].set("field", "Table Name");
	t.block("header_col")[1].set("field", "Column count");

    if (size != 0) {
    	NL::Template::Block & block = t.block( "row" );
		/* fill remaining rows */
		for (int i=0;i<size;++i) {

			block[i].block( "col" ).repeat( 2 );
			/* fill columns in a specific row */
			block[i].block("col")[0].set("field", "<a href=/?table=" + result.getRow(i)[0]
						+"&pgsize="+configuration.getPageSize()+"&pgnum=0>"+result.getRow(i)[0]+"</a>");
			block[i].block("col")[1].set("field", result.getRow(i)[1]);

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
