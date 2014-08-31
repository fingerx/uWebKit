/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "ThirdParty/sqlite/sqlite3.h"
#include <string>
#include <vector>
#include "uwk_common.h"
#include "uwk_process_common.h"


// multiple instances of the same application may be running locally

// server table of active servers

// client table of active clients

class UWKProcessServer;
class UWKProcessClient;

class UWKProcessDB
{

    struct QueryRow
    {
        std::vector<std::string> columns;
        std::vector<std::string> values;
    };

    typedef std::vector<QueryRow> QueryResult;

    sqlite3* database_;

    static UWKProcessDB* sInstance_;

    static void SQLiteError(const char *format, const char* sqliteErrorMsg);

    void DropTables();
    bool CreateTables();
    bool InitDB(const std::string& dbPath);

    UWKProcessDB(bool server);

    static int QueryCallback(void* ptr, int argc , char** values, char** columns);

    // bump to invalidate the process database for changing tables
    static std::string sVersion_;

    // there are multiple servers
    bool server_;

    void ReapClient(unsigned long parentPID);

    // returns first available server id
    int RefreshServers();

    // gets the ids of the current running servers
    void GetActiveServerIds(std::vector<unsigned long>& pids, std::vector<int>& ids);

public:

    void RegisterServer(UWKProcessServer* server);
    void RegisterClient(UWKProcessClient* client);

    bool GetServerConfig(const UWKProcessCommon::PID& pid, std::string& config);

    bool CheckProcessTerminated(const UWKProcessCommon::PID& pid, bool serverProcess);

    bool CheckProcessTimeout(const UWKProcessCommon::PID& pid, bool serverProcess, bool& terminated);

    bool UpdateServerTimestamp(const UWKProcessCommon::PID& pid);
    bool UpdateClientTimestamp(const UWKProcessCommon::PID& pid);

    static UWKProcessDB* Instance();
    static bool Initialize(const std::string& dbPath,  bool server);
    static void Shutdown();


};

