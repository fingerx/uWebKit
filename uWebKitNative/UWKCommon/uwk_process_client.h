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


class UWKProcessClient : public UWKProcessCommon
{
    static UWKProcessClient* sInstance_;
    UWKProcessClient(const PID& pid, const PID& parentPID);

public:

    PID parentPID_;
    static UWKProcessClient* Instance();
    static bool Initialize(const PID& parentPID, const std::string& processdb);
    bool Update();
    static void Shutdown();

};
