/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "Poco/Process.h"
#include "Poco/Timestamp.h"
#include "uwk_common.h"

using Poco::Process;
using Poco::ProcessHandle;
using Poco::Timestamp;

// each page gets it's own process, page state is persistent in db
// so if page process hangs/crashes can be killed/restarted and restore state
// the server is going to know about the client crash/hang and will be doing the
// restarting

// client and server talk through the sqlite ProcessDB
// client and server have uniqueID based on PID

// restored page javascript bridge also needs to be persistent?


class UWKProcessCommon
{

protected:

    Timestamp updateTime_;

public:

    typedef Poco::ProcessImpl::PIDImpl PID;
    PID pid_;

    UWKProcessCommon(const PID& pid) : pid_(pid)
    {

    }

};
