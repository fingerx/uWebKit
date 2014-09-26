/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <stdlib.h>
#include <stdarg.h>

typedef void (*UWKLogCallback)(const char* string, int logLevel);

enum UWKLogLevel
{
    UWK_LOG_VERBOSE,
    UWK_LOG_WARNING
};

class UWKLog
{
    static UWKLogCallback sLogCallback;

public:

    static void SetLogCallback(UWKLogCallback callback);

    static void LogVerbose(const char *format, ...);

};

