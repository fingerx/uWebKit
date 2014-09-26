/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <sstream>
#include "Poco/Path.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "uwk_config.h"
#include "uwk_process_server.h"
#include "uwk_processdb.h"
#include "uwk_process_utils.h"

using Poco::Path;
using Poco::Base64Encoder;
using Poco::Base64Decoder;

UWKProcessServer* UWKProcessServer::sInstance_ = NULL;

UWKProcessServer::UWKProcessServer(const PID& pid) : UWKProcessCommon(pid),
    renderProcessHandle_(NULL),
    renderProcessPID_(0),
    serverID_(0)
{

}

UWKProcessServer::~UWKProcessServer()
{

    if (renderProcessHandle_)
        delete renderProcessHandle_;

}

bool UWKProcessServer::Update()
{
    if (!renderProcessHandle_)
        return false;

    // every 5 seconds update the timestamp in the process db
    Timestamp::TimeDiff diff = (Timestamp() - updateTime_) / 1000;
    if (diff > (5000 * 1))
    {
        updateTime_ = Timestamp();

        UWKProcessDB::Instance()->UpdateServerTimestamp(Process::id());

        if (renderProcessPID_)
        {
            bool terminated;
            if (UWKProcessDB::Instance()->CheckProcessTimeout(renderProcessPID_, false, terminated))
            {
                if (!terminated)
                {
                    // kill and then update in 2 seconds
                    updateTime_ -= 1000 * 3000;
                    UWKLog::LogVerbose("UWKProcess has timed out, sending kill");
                    Poco::Process::kill(*renderProcessHandle_);
                    return false;
                }
                else
                {
                    UWKLog::LogVerbose("UWKProcess has exited");
                }

                return true;
            }
        }
    }

    return false;

}

bool UWKProcessServer::SpawnClient(const Process::Args& args)
{

    if (renderProcessHandle_)
    {
        delete renderProcessHandle_;
        renderProcessHandle_ = NULL;
    }

    // capture args for restart
    renderProcessArgs_ = args;

    std::string command;
    UWKConfig::GetWebRenderProcessPath(command);

    std::string initialDirectory;
    UWKConfig::GetWebRenderProcessWorkingPath(initialDirectory);

    UWKProcessUtils::EnsureExecutable(command);

    renderProcessPID_ = 0;

    char pid[256];
#ifdef _MSC_VER
    _snprintf(pid, 250, "%u", (unsigned int) pid_);
#else
    snprintf(pid, 250, "%u", (unsigned int) pid_);
#endif

    std::vector<std::string> nargs = renderProcessArgs_;
    nargs.push_back("-parentpid");
    nargs.push_back(pid);

    nargs.push_back("-processdb");

    std::string path;
    UWKConfig::GetPersistentDataPath(path);
    path += "/uWebKitProcess.db";

    std::ostringstream processdb64;
    Base64Encoder encoder(processdb64);
    encoder << path;
    encoder.close();

    nargs.push_back(processdb64.str());

    renderProcessHandle_ = new ProcessHandle(Process::launch(command, nargs, initialDirectory));

    if (!renderProcessHandle_->id())
    {
        delete renderProcessHandle_;
        renderProcessHandle_ = NULL;
        return false;
    }

    renderProcessPID_ = renderProcessHandle_->id();

    return true;

}

UWKProcessServer* UWKProcessServer::Instance()
{
    if (!sInstance_)
    {
        UWKError::FatalError("UWKProcessServer not initialized");
        return NULL;
    }

    return sInstance_;

}

void UWKProcessServer::Initialize()
{
    if (sInstance_)
    {
        UWKError::FatalError("UWKProcessServer already initialized");
        return;
    }

    sInstance_ = new UWKProcessServer(Process::id());

    std::string path;
    UWKConfig::GetPersistentDataPath(path);
    path += "/uWebKitProcess.db";

    UWKProcessDB::Initialize(path, true);

    UWKConfig::SetServerID(0);

    UWKProcessDB::Instance()->RegisterServer(sInstance_);

    UWKConfig::SetServerID(sInstance_->serverID_);

}

void UWKProcessServer::Shutdown()
{
    if (!sInstance_)
        return;

    UWKProcessDB::Shutdown();

    delete sInstance_;
    sInstance_ = NULL;
}
