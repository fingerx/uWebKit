/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_error.h"
#include "uwk_config.h"
#include "uwk_processdb.h"
#include "uwk_process_client.h"

UWKProcessClient* UWKProcessClient::sInstance_ = NULL;

UWKProcessClient::UWKProcessClient(const PID &pid, const PID &parentPID) :
    UWKProcessCommon(pid),
    parentPID_(parentPID)
{

}

UWKProcessClient* UWKProcessClient::Instance()
{
    if (!sInstance_)
    {
        UWKError::FatalError("UWKProcessClient not initialized");
        return NULL;
    }

    return sInstance_;

}

bool UWKProcessClient::Update()
{
    // every 5 seconds update the timestamp in the process db and check for server timeout
    Timestamp::TimeDiff diff = (Timestamp() - updateTime_) / 1000;

    if (diff > (5000 * 1))
    {
        bool terminated;
        if (UWKProcessDB::Instance()->CheckProcessTimeout(parentPID_, true, terminated))
        {
            // the server is non-responsive, it could be in background or it maybe has a menu
            // item open in Windows/OSX (which freezes main loop), we will only
            // report if server process has actually terminated here
            if (terminated)
                return false;
        }

        UWKProcessDB::Instance()->UpdateClientTimestamp(Process::id());
        updateTime_ = Timestamp();
    }

    return true;

}


bool UWKProcessClient::Initialize(const PID& parentPID, const std::string& processdb)
{
    if (sInstance_)
    {
        UWKError::FatalError("UWKProcessClient already initialized");
        return false;
    }

    if (!UWKProcessDB::Initialize(processdb, false))
        return false;

    std::string config;
    if (!UWKProcessDB::Instance()->GetServerConfig(parentPID, config))
        return false;

    if (!UWKConfig::SetFromJSON(config))
        return false;

    sInstance_ = new UWKProcessClient(Process::id(), parentPID);

    UWKProcessDB::Instance()->RegisterClient(sInstance_);

    return true;

}

void UWKProcessClient::Shutdown()
{
    if (!sInstance_)
        return;

    UWKProcessDB::Shutdown();

    delete sInstance_;
    sInstance_ = NULL;

}
