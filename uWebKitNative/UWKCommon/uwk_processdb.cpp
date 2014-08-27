/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <sstream>
#include "uwk_config.h"
#include "uwk_processdb.h"
#include "uwk_process_client.h"
#include "uwk_process_server.h"
#include "uwk_process_utils.h"

#ifndef _MSC_VER
// for kill()
#include <signal.h>
#endif


UWKProcessDB* UWKProcessDB::sInstance_ = NULL;
std::string UWKProcessDB::sVersion_  = "1.0";

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
    ss.clear();

    int rc = sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    return (rc == SQLITE_OK);

}

bool UWKProcessDB::CheckProcessTerminated(const UWKProcessCommon::PID& pid, bool serverProcess)
{
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
    ss << " AND timestamp < datetime('now','-15 seconds');";

    char* errMsg = NULL;
    QueryResult result;
    int rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc != SQLITE_OK)
    {
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
    ss.clear();

    int rc = sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    return (rc == SQLITE_OK);

}

void UWKProcessDB::ReapClients()
{

    std::stringstream ss;
    ss << "SELECT pid FROM clients;";

    char* errMsg = NULL;
    QueryResult result;
    int rc = sqlite3_exec(database_, ss.str().c_str(), QueryCallback, &result, &errMsg );
    if ( rc != SQLITE_OK)
    {
        SQLiteError("Error ReapClientsUsingServerPID: %s", errMsg);
        return;
    }

    std::string webRenderProcessPath;

    UWKConfig::GetWebRenderProcessPath(webRenderProcessPath);

    for (size_t i = 0; i < result.size(); i++)
    {
        UWKProcessCommon::PID cpid = (UWKProcessCommon::PID) strtoul(result[i].values[0].c_str(), NULL, 0);

        std::string path;
        if (UWKProcessUtils::GetExecutablePath(cpid, path) || !path.length())
        {
            if (UWKProcessUtils::CompareExecutablePaths(path, webRenderProcessPath))
            {
                 Poco::Process::kill(cpid);
            }

        }

    }

}

void UWKProcessDB::RegisterServer(UWKProcessServer* server)
{
    ReapClients();

    std::stringstream ss;

    ss << "DELETE FROM servers WHERE pid = ";
    ss << server->pid_;
    ss << ";";
    std::string sql =  ss.str();
    ss.clear();
    sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    ss << "DELETE FROM clients WHERE parentpid = ";
    ss << server->pid_;
    ss << ";";
    sql =  ss.str();
    ss.clear();
    sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    std::string config;
    UWKConfig::GetJSON(config);

    ss << "INSERT INTO servers (pid, config) VALUES ( ";
    ss << server->pid_;
    ss << ", '";
    ss << config;
    ss << "' );";

    sql =  ss.str();
    ss.clear();
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
    ss.clear();
    sqlite3_exec(database_, sql.c_str(), NULL, NULL, NULL);

    ss << "INSERT INTO clients (pid, parentpid) VALUES ( ";
    ss << client->pid_;
    ss << ", '";
    ss << client->parentPID_;
    ss << "' );";

    sql =  ss.str();
    ss.clear();
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
    rc = sqlite3_exec(database_, "CREATE TABLE dbinfo ( version TEXT NOT NULL );", NULL, NULL, &errMsg );
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
    rc = sqlite3_exec(database_, "CREATE TABLE servers ( pid INTEGER PRIMARY KEY NOT NULL, config TEXT NOT NULL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP );", NULL, NULL, &errMsg );
    if (rc != SQLITE_OK)
    {
        SQLiteError("Error creating servers table for process database: %s", errMsg);
        return false;
    }

    // create the clients table
    rc = sqlite3_exec(database_, "CREATE TABLE clients ( pid INTEGER PRIMARY KEY NOT NULL, parentpid INTEGER NOT NULL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);", NULL, NULL, &errMsg );
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
