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
        return handleRequestError("Invalid request");
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
    
    try
    {
        
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
                auto pkVector = dataBase.getPrimaryKeyColumnName(itTable->second);
                auto namePk = pkVector.front();
                auto itPk = request.getParameters().find(namePk);
                if(request.getParameters().end() != itPk) // czy jest parametr klucza glownego
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
    
    }
    catch (const std::exception &ex)
    {
        return handle404Error(request);
    }
     
     return handle404Error(request);
}

Response FancyHandler::handle404Error(const Request& request) const {

    Response response;
    std::ostringstream msgStream;
    NL::Template::LoaderFile loader;
    NL::Template::Template t( loader );
    t.load( configuration.getRootResDir()+"templates/404.html" );

    t.set("rootResDir", configuration.getRootResDir());
    t.set("head_title", "404 Not Found");
    t.set("title", "404 Not Found");
    t.set("alltable_path","/alltables?pgsize="+configuration.getPageSize()+"&pgnum=0");
    t.set("alltable_description", "All Tables");
    t.set("table_description", "Refresh");

    t.block("prev_btn").disable();
    t.block("next_btn").disable();

    t.set("message", "The requested URL "+ request.getResource() +" was not found on this server.");
    t.render( msgStream );

    response.message=msgStream.str();
    response.code = 200;
    response.headers["Content-Type"] = "text/html";
    
    return response;
}

Response FancyHandler::handleFetchResource(const Request& request, const std::string& path) const {
    Response response;
    std::string content_type = path.substr(path.find_last_of(".")+1);
    LOGD("Sciezka: "+path+", typ: "+content_type);
    if (content_type!="js" and content_type!="css")
    	return FancyHandler::handleRequestError("No such file or directory.");


    std::ifstream in(configuration.getRootResDir()+path);
    std::string msg((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    response.message=msg;
    response.code = 200;
    response.headers["Content-Type"] = "text/"+content_type;

    return response;
}


Response FancyHandler::handleSuccessDetails(const Request& request, const std::string tableName) const {
    Response response;
    std::ostringstream msgStream;
    NL::Template::LoaderFile loader;
    NL::Template::Template t( loader );
    t.load( configuration.getRootResDir()+"templates/detail.html" );

    t.set("rootResDir", configuration.getRootResDir());
    t.set("head_title", "Details for table " + tableName);
    t.set("title", tableName);
    t.set("pgsize", configuration.getPageSize());
    t.set("pgnum", "0");
    t.set("table_name",tableName);
    t.set("table_description", tableName);

	t.block("prev_btn").disable();
	t.block("next_btn").disable();

    std::string columnName = *(dataBase.getPrimaryKeyColumnName(tableName).begin());
    auto it = request.getParameters().find(columnName);
    std::string columnValue = it->second;
    std::string sql = "select * from " + tableName + " where " + columnName + " = \'" + columnValue + "\'";
    Table result = dataBase.execSelect(sql).second;

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
    t.set("head_title", "Table \"" + tableName + "\"");
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

	const std::vector<std::string> pkNames = dataBase.getPrimaryKeyColumnName(tableName);

    std::pair<bool,Table> resultPair = dataBase.execSelect(sql,pkNames, pgSize,pgNum);
    Table result = resultPair.second;
    bool isLastPage = resultPair.first;

    setFooterTemplate(t, "/?table="+tableName+"&", pgSize,pgNum,isLastPage);

    int size = result.tableSize();
    int col_num = result.rowSize();
    
    std::tuple<bool,std::string,std::string> isFKcolumn[col_num];
    std::string pkColName = *(dataBase.getPrimaryKeyColumnName(tableName).begin());

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
        NL::Template::Block & colBlock = t.block( "header_col" );
        if(std::get<0>(isFKcolumn[j]))
        {
        	colBlock[j].block("header_simple_col").disable();
        	colBlock[j].block("header_link").set("field", colName);
        	colBlock[j].block("header_link").set("pgsize", std::to_string(pgSize));
        	colBlock[j].block("header_link").set("trg_table_name", std::get<1>(isFKcolumn[j]));
        }
        else
        {
        	colBlock[j].block("header_link").disable();
        	colBlock[j].block("header_simple_col").set("field", colName);
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
				NL::Template::Block & colBlock = block[i].block( "col" );
				if(std::get<0>(isFKcolumn[j])) {
					colBlock[j].block("simple_val").disable();
					colBlock[j].block("link").set("field", cell);
					colBlock[j].block("link").set("pgsize", std::to_string(pgSize));
					colBlock[j].block("link").set("trg_table_name", std::get<1>(isFKcolumn[j]));
					colBlock[j].block("link").set("trg_col_name", std::get<2>(isFKcolumn[j]));
					colBlock[j].block("link").set("trg_col_value", cell);
				}
				else if (pkColNum == j) {
					colBlock[j].block("simple_val").disable();
					colBlock[j].block("link").set("field", cell);
					colBlock[j].block("link").set("pgsize", std::to_string(pgSize));
					colBlock[j].block("link").set("trg_table_name", tableName);
					colBlock[j].block("link").set("trg_col_name", result.getColumnsNames()[j]);
					colBlock[j].block("link").set("trg_col_value", cell);
				}
				else
				{
					colBlock[j].block("link").disable();
					colBlock[j].block("simple_val").set("field", cell);
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
    t.set("title", "List of all tables");
    t.set("head_title", "List of all tables");
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

	std::vector<string> orderByColNames = std::vector<string>();
	orderByColNames.push_back("table_name");

    std::pair<bool,Table> resultPair = dataBase.execSelect(sql,orderByColNames, pgSize,pgNum);
    Table result = resultPair.second;
    bool isLastPage = resultPair.first;

    setFooterTemplate(t, "/alltables?", pgSize,pgNum,isLastPage);

    int size = result.tableSize();


    /* fill header row fields */
    t.block( "header_col" ).repeat( 2 );
	t.block("header_col")[0].set("field", "Table Name");
	t.block("header_col")[1].set("field", "Column count");

	t.block( "row" ).repeat( size );
    if (size != 0) {
    	NL::Template::Block & block = t.block( "row" );
    	std::string pgSizeStr = configuration.getPageSize();
		/* fill remaining rows */
		for (int i=0;i<size;++i) {
			/* fill columns in a specific row */
			block[i].set("pgsize", pgSizeStr);
			block[i].set("table_name", result.getRow(i)[0]);
			block[i].set("table_count", result.getRow(i)[1]);
			LOGD("w FOR: "+result.getRow(i)[0]+" "+result.getRow(i)[1]);
		}
    }
    t.render( msgStream );
    response.message=msgStream.str();
    response.code = 200;
    response.headers["Content-Type"] = "text/html";

    return response;
}


Response FancyHandler::handleSuccessEcho(const Request& request) const {
    Response response;

    response.code = 200;
    constexpr char echo[] = "/echo/";

    fillSimpleResponse(response, request, request.getResource().substr(sizeof(echo) - 1));

    return response;
}

// internal handlers not required by the server API
server::Response FancyHandler::handleRequestError(const std::string & message) const {
    Response response;

    response.code = 400;
    response.headers["Content-Type"] = "text/plain";
    response.message = message;

    return response;
}

Response FancyHandler::handleGeneralError(const Request& request) const {
    Response response;

    response.code = 400;

    constexpr auto message = "Invalid request.";
    fillSimpleResponse(response, request, message);

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

void FancyHandler::setFooterTemplate(const NL::Template::Template & t, const std::string & base_url, const int pgSize, const int pgNum, const bool isLastPage) const {

	t.block("prev_btn").set("base_url", base_url);
	t.block("prev_btn").set("pgsize", std::to_string(pgSize));
	t.block("prev_btn").set("pgnum",std::to_string(pgNum-1));

	t.block("next_btn").set("base_url", base_url);
	t.block("next_btn").set("pgsize", std::to_string(pgSize));
	t.block("next_btn").set("pgnum",std::to_string(pgNum+1));

    if (pgNum<=0 )
    	t.block("prev_btn").disable();
	if (isLastPage or pgSize<0)
		t.block("next_btn").disable();
}
