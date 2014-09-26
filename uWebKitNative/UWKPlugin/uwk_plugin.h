/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "UWKCommon/uwk_server.h"

class UWKPlugin
{
public:


    static bool sShutDown_;
    static int sBetaDaysLeft_;
    static bool sHasDisplayedBetaMessage_;
    static void Initialize(UWKProcessMessage processCB, const char* utf8JsonConfig);
    static void Shutdown();

};
