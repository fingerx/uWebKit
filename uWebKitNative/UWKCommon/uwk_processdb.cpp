/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "jansson.h"
#include <sstream>
#include <algorithm>
#include "uwk_config.h"
#include "uwk_processdb.h"
#include "uwk_process_client.h"
#include "uwk_process_server.h"
#include "uwk_process_utils.h"
#include "Poco/String.h"

#ifndef _MSC_VER
// for kill()
#include <signal.h>
#endif


UWKProcessDB* UWKProcessDB::sInstance_ = NULL;
std::string UWKProcessDB::sVersion_  = "1.06";

UWKProcessDB::UWKProcessDB(bool server) : database_ (NULL), server_(server)
{

}

bool UWKProcessDB::GetServerConfig(const UWKProcessCommon::PID& pid, std::string& config)
{
    if (!database_)
        return false;

    std::stringstream ss;
    ss << "SELECT config FROM servers WHERE pid = ";
    ss << pid;
    ss << ";";

    char* errMsg = NULL;
    QueryResult result;
    int rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc != SQLITE_OK || result.size() != 1)
    {
        SQLiteError("Error querying server config: %s", errMsg);
        return false;
    }

    config = result[0].values[0];

    return true;
}

bool UWKProcessDB::UpdateServerTimestamp(const UWKProcessCommon::PID& pid)
{
    std::stringstream ss;

    ss << "UPDATE servers SET timestamp = datetime('now') WHERE pid = ";
    ss << pid;
    ss << ";";
    std::string sql =  ss.str();
    ss.str(std::string());

    int rc = sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    return (rc == SQLITE_OK);

}

bool UWKProcessDB::CheckProcessTerminated(const UWKProcessCommon::PID& pid, bool serverProcess)
{

#ifdef UWK_WINDBG
    return false;
#endif
    
    std::string path;

    if (!UWKProcessUtils::GetExecutablePath(pid, path) || !path.length())
    {
        return true;
    }

    std::string serverProcessPath;
    std::string webRenderProcessPath;

    UWKConfig::GetServerProcessPath(serverProcessPath);
    UWKConfig::GetWebRenderProcessPath(webRenderProcessPath);

    if (serverProcess)
    {
        //UWKLog::LogVerbose("Compare %s : %s", serverProcessPath.c_str(), path.c_str() );

        if (!UWKProcessUtils::CompareExecutablePaths(serverProcessPath, path))
            return true;
    }
    else
    {
        //UWKLog::LogVerbose("Compare %s : %s", webRenderProcessPath.c_str(), path.c_str());
        if (!UWKProcessUtils::CompareExecutablePaths(webRenderProcessPath, path))
            return true;
    }

    return false;

}


bool UWKProcessDB::CheckProcessTimeout(const UWKProcessCommon::PID& pid, bool serverProcess, bool &terminated)
{
    terminated = false;

#ifdef UWK_WINDBG
    return false;
#endif

    // first check that the server is running
    if (CheckProcessTerminated(pid, serverProcess))
    {
        terminated = true;
        return true;
    }

    std::stringstream ss;
    ss << "SELECT pid FROM ";
    if (!serverProcess)
    {
        ss << "clients ";
    }
    else
    {
        ss << "servers ";
    }

    ss << "WHERE pid = ";
    ss << pid;
    ss << " AND timestamp < datetime('now','-25 seconds');";

    char* errMsg = NULL;
    QueryResult result;
    int rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc != SQLITE_OK)
    {
        if (rc == SQLITE_BUSY)
        {
            return false;
        }

        SQLiteError("Error checking server timeout: %s", errMsg);
        return true;
    }

    // server has timed out
    if (result.size() == 1)
    {
        // the pid is hung
        return true;
    }

    return false;

}


bool UWKProcessDB::UpdateClientTimestamp(const UWKProcessCommon::PID& pid)
{
    std::stringstream ss;

    ss << "UPDATE clients SET timestamp = datetime('now') WHERE pid = ";
    ss << pid;
    ss << ";";
    std::string sql =  ss.str();
    ss.str(std::string());

    int rc = sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    return (rc == SQLITE_OK);

}

int UWKProcessDB::RefreshServers()
{
    std::vector<int> ids;
    std::vector<unsigned long> pids;
    GetActiveServerIds(pids, ids);

    std::stringstream ss;
    ss << "SELECT pid FROM servers;";

    char* errMsg = NULL;
    QueryResult result;
    int rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc != SQLITE_OK)
    {
        SQLiteError("Error RefreshServers: %s", errMsg);
        return 0;
    }

    ss.str(std::string());

    for (size_t i = 0; i < result.size(); i++)
    {
        unsigned long pid = strtoul(result[i].values[0].c_str(), NULL, 0);

        if (std::find(pids.begin(), pids.end(), pid) == pids.end())
        {
            ss << "DELETE FROM servers WHERE pid = ";
            ss << pid;
            ss << ";";
            std::string sql =  ss.str();
            ss.str(std::string());
            sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);
        }

    }

    // Reap orphaned clients
    ss << "SELECT parentpid FROM clients;";
    result.clear();
    rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc == SQLITE_OK)
    {
        for (size_t i = 0; i < result.size(); i++)
        {
            unsigned long parentpid = strtoul(result[i].values[0].c_str(), NULL, 0);

            if (std::find(pids.begin(), pids.end(), parentpid) == pids.end())
            {
                ReapClient(parentpid);
            }
        }
    }

    ss.str(std::string());

    int i = 1;
    while (true)
    {
        if (std::find(ids.begin(), ids.end(), i) == ids.end())
            break;
        i++;
    }

    return i;


}

void UWKProcessDB::GetActiveServerIds(std::vector<unsigned long> &pids, std::vector<int>& ids)
{
    std::stringstream ss;
    ss << "SELECT pid, id, config FROM servers;";

    char* errMsg = NULL;
    QueryResult result;
    int rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc != SQLITE_OK)
    {
        SQLiteError("Error GetActiveServerCount: %s", errMsg);
        return;
    }

    std::string serverProcessPath;

    UWKConfig::GetServerProcessPath(serverProcessPath);

    for (size_t i = 0; i < result.size(); i++)
    {
        UWKProcessCommon::PID cpid = (UWKProcessCommon::PID) strtoul(result[i].values[0].c_str(), NULL, 0);
        unsigned long id = strtoul(result[i].values[1].c_str(), NULL, 0);

        std::string path;
        if (UWKProcessUtils::GetExecutablePath(cpid, path) && path.length())
        {
            // this pid is alive
            json_error_t error;
            json_t* json = json_loads(result[i].values[2].c_str(), JSON_DISABLE_EOF_CHECK, &error);

            if (!json)
            {
                continue;
            }

            json_t* appConfig = json_object_get(json, "application");

            if (!appConfig)
            {
                json_decref(json);
                continue;
            }

             json_t* serverProcessPath = json_object_get(appConfig, "serverProcessPath");

             if (!json_is_string(serverProcessPath))
             {
                 json_decref(json);
                 continue;
             }

            if (UWKProcessUtils::CompareExecutablePaths(path, json_string_value(serverProcessPath)))
            {
                pids.push_back((unsigned long) cpid);
                ids.push_back((int) id);
            }

        }

    }

    return;

}

void UWKProcessDB::ReapClient(unsigned long parentPID)
{

    std::stringstream ss;
    ss << "SELECT pid, config FROM clients WHERE parentpid = ";
    ss << parentPID;
    ss << ";";

    char* errMsg = NULL;
    QueryResult result;
    int rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc != SQLITE_OK)
    {
        SQLiteError("Error ReapClientsUsingServerPID: %s", errMsg);
        return;
    }

    ss.str(std::string());

    bool killAttempted = false;

    for (size_t i = 0; i < result.size(); i++)
    {
        UWKProcessCommon::PID cpid = (UWKProcessCommon::PID) strtoul(result[i].values[0].c_str(), NULL, 0);

        std::string path;
        if (UWKProcessUtils::GetExecutablePath(cpid, path) && path.length())
        {
            // this pid is alive
            json_error_t error;
            json_t* json = json_loads(result[i].values[1].c_str(), JSON_DISABLE_EOF_CHECK, &error);

            if (!json)
            {
                continue;
            }

            json_t* appConfig = json_object_get(json, "application");

            if (!appConfig)
            {
                json_decref(json);
                continue;
            }

             json_t* webRenderProcessPath = json_object_get(appConfig, "webRenderProcessPath");

             if (!json_is_string(webRenderProcessPath))
             {
                 json_decref(json);
                 continue;
             }

            if (UWKProcessUtils::CompareExecutablePaths(path, json_string_value(webRenderProcessPath)))
            {
                killAttempted = true;
                Poco::Process::kill(cpid);
            }

        }
    }

    if (!killAttempted)
    {
        ss << "DELETE FROM clients WHERE parentpid = ";
        ss << parentPID;
        ss << ";";
        sqlite3_exec(database_, ss.str().c_str(), NULL, NULL, NULL);
        ss.str(std::string());
    }

}

void UWKProcessDB::RegisterServer(UWKProcessServer* server)
{

    // we can only have one client
    ReapClient(server->pid_);


    server->serverID_ = RefreshServers();
    UWKConfig::SetServerID(server->serverID_);

    std::stringstream ss;

    ss << "DELETE FROM servers WHERE pid = ";
    ss << server->pid_;
    ss << ";";
    std::string sql =  ss.str();
    ss.str(std::string());
    sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    std::string config;
    UWKConfig::GetJSON(config);

    // escape ' to '' for SQL
    config = Poco::replace(config, std::string("'"), std::string("''"));

    ss << "INSERT INTO servers (pid, config, id) VALUES ( ";
    ss << server->pid_;
    ss << ", '";
    ss << config;
    ss << "', ";
    ss << server->serverID_;
    ss << " );";
    sql =  ss.str();
    ss.str(std::string());
    sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

}

void UWKProcessDB::RegisterClient(UWKProcessClient* client)
{

    std::stringstream ss;
    std::string sql;

    ss << "DELETE FROM clients WHERE pid = ";
    ss << client->pid_;
    ss << ";";
    sql =  ss.str();
    ss.str(std::string());
    sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    std::string config;
    UWKConfig::GetJSON(config);

    ss << "INSERT INTO clients (pid, config, parentpid) VALUES ( ";
    ss << client->pid_;
    ss << ", '";
    ss << config;
    ss << "', ";
    ss << client->parentPID_;
    ss << " );";
    sql =  ss.str();

    sql =  ss.str();
    ss.str(std::string());
    sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);
}


UWKProcessDB* UWKProcessDB::Instance()
{
    if (!sInstance_)
    {
        UWKError::FatalError("UWKProcessDB not initialized");
    }

    return sInstance_;

}

int UWKProcessDB::QueryCallback(void* ptr,int argc ,char** values, char** columns)
{
    QueryResult* result = (QueryResult*) ptr;

    QueryRow row;
    result->push_back(row);

    for (int i = 0; i < argc; i++)
    {
        result->back().columns.push_back(columns[i]);
        result->back().values.push_back(values[i]);
    }

    return 0;
}

void UWKProcessDB::DropTables()
{
    if (!server_)
    {
        UWKError::FatalError("Process database drop tables called on client");
        return;
    }

    sqlite3_exec(database_, "DROP TABLE dbinfo;", NULL, NULL, NULL);
    sqlite3_exec(database_, "DROP TABLE servers;", NULL, NULL, NULL);
    sqlite3_exec(database_, "DROP TABLE clients;", NULL, NULL, NULL);

}

bool UWKProcessDB::CreateTables()
{

    if (!server_)
    {
        UWKError::FatalError("Process database create tables called on client");
        return false;
    }

    int rc;
    char* errMsg = NULL;

    DropTables();

    // create the db info table
    rc = sqlite3_exec(database_, "CREATE TABLE dbinfo ( version TEXT NOT NULL, activationpid INTEGER DEFAULT 0 );", NULL, NULL, &errMsg );
    if (rc != SQLITE_OK)
    {
        SQLiteError("Error creating dbinfo table for process database: %s", errMsg);
        return false;
    }

    // store the version
    std::string sql = "INSERT INTO dbinfo ( version ) VALUES ('" + sVersion_+ "');";
    rc = sqlite3_exec(database_, sql.c_str(), NULL, NULL, &errMsg );
    if (rc != SQLITE_OK)
    {
        SQLiteError("Error initializing dbinfo table data for process database: %s", errMsg);
        return false;
    }

    // create the servers table
    rc = sqlite3_exec(database_, "CREATE TABLE servers ( pid INTEGER PRIMARY KEY NOT NULL, config TEXT NOT NULL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, id INTEGER DEFAULT 0);", NULL, NULL, &errMsg );
    if (rc != SQLITE_OK)
    {
        SQLiteError("Error creating servers table for process database: %s", errMsg);
        return false;
    }

    // create the clients table
    rc = sqlite3_exec(database_, "CREATE TABLE clients ( pid INTEGER PRIMARY KEY NOT NULL, parentpid INTEGER NOT NULL, config TEXT NOT NULL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);", NULL, NULL, &errMsg );
    if (rc != SQLITE_OK)
    {
        SQLiteError("Error creating clients table for process database: %s", errMsg);
        return false;
    }

    return true;

}

void UWKProcessDB::SQLiteError(const char* format, const char *sqliteErrorMsg)
{
    if (sqliteErrorMsg)
    {
        UWKError::FatalError(format, sqliteErrorMsg);
        sqlite3_free((void*) sqliteErrorMsg);
    }
    else
    {
        UWKError::FatalError(format);
    }
}

bool UWKProcessDB::InitDB(const std::string& dbPath)
{
    int rc = sqlite3_open(dbPath.c_str(), &database_);

    if (rc != SQLITE_OK)
    {
        UWKError::FatalError("Can't open process database: %s", sqlite3_errmsg(database_));
        sqlite3_close(database_);
        return false;
    }

    // TODO: check integrity with SQLite pragma

    if (!server_)
        return true;

    // The server checks if DB exists and whether it needs to create tables/re-init outdated database
    char* errMsg = NULL;
    QueryResult result;
    rc = sqlite3_exec(database_, "SELECT name FROM sqlite_master WHERE type='table' AND name='dbinfo';", QueryCallback, &result, &errMsg );

    if (rc != SQLITE_OK)
    {
        SQLiteError("Error querying process database for table info: %s", errMsg);
        return false;
    }

    if (!result.size())
    {
        // We need to create tables
        CreateTables();
    }

    // check DB version
    result.clear();
    rc = sqlite3_exec(database_, "SELECT version FROM dbinfo", QueryCallback, &result, &errMsg );
    if (rc != SQLITE_OK)
    {
        SQLiteError("Error querying process database version: %s", errMsg);
        return false;
    }

    // check problem with DB or whether version mismatch
    // for now, always drop
    if (result.size() != 1 || result[0].values[0] != sVersion_)
    {
        if (!CreateTables())
            return false;

        result.clear();
        rc = sqlite3_exec(database_, "SELECT version FROM dbinfo", QueryCallback, &result, &errMsg );
        if (rc != SQLITE_OK)
        {
            SQLiteError("Error querying process database version, after re-created tables: %s", errMsg);
            return false;
        }
    }

    return true;

}

void UWKProcessDB::GetActivationServerPID(UWKProcessCommon::PID& pid)
{
    char* errMsg = NULL;
    QueryResult result;

    int rc = sqlite3_exec(database_, "SELECT activationpid FROM dbinfo", QueryCallback, &result, &errMsg );

    if (rc != SQLITE_OK || result.size() != 1)
    {
        pid = (UWKProcessCommon::PID) 0;
        return;
    }

    pid = (UWKProcessCommon::PID) strtoul(result[0].values[0].c_str(), NULL, 0);

}

void UWKProcessDB::SetActivationServerPID(const UWKProcessCommon::PID& pid)
{
    std::stringstream ss;

    ss << "UPDATE dbinfo SET activationpid = ";
    ss << pid;
    ss << ";";
    std::string sql =  ss.str();
    ss.str(std::string());

    char* errMsg = NULL;
    int rc = sqlite3_exec(database_, sql.c_str(), NULL, NULL, &errMsg);

    if (rc != SQLITE_OK)
    {
        UWKLog::LogVerbose("%s", errMsg);
    }

}

bool UWKProcessDB::Initialize(const std::string &dbPath, bool server)
{
    if (sInstance_)
    {
        UWKError::FatalError("UWKProcessDB already initialized");
        return false;
    }

    sInstance_ = new UWKProcessDB(server);

    return sInstance_->InitDB(dbPath);
}

void UWKProcessDB::Shutdown()
{
    if (!sInstance_)
        return;

    sqlite3_close(sInstance_->database_);

    delete sInstance_;

    sInstance_ = NULL;

}
