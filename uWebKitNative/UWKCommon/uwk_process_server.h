/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "uwk_process_common.h"

// there is only one server
class UWKProcessServer : public UWKProcessCommon
{
    static UWKProcessServer* sInstance_;
    UWKProcessServer(const PID& pid);
    ~UWKProcessServer();

    ProcessHandle *renderProcessHandle_;
    Process::Args renderProcessArgs_;

public:

    PID renderProcessPID_;

    // globally unique ID as more than once server (application instance) may be running
    // valid once server is registered
    int serverID_;

    static UWKProcessServer* Instance();

    bool Update();
    bool SpawnClient(const Process::Args& args);

    static void Initialize();
    static void Shutdown();
    static bool MakeExecutable(const std::string& command);

};
