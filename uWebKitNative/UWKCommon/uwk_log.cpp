/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <stdio.h>
#include "uwk_log.h"


UWKLogCallback UWKLog::sLogCallback = NULL;

void UWKLog::SetLogCallback(UWKLogCallback callback)
{
    sLogCallback = callback;
}

void UWKLog::LogVerbose(const char *format, ...)
{

    if (!sLogCallback)
        return;

    char buff[4096];
    va_list args;

    va_start(args, format);
    vsnprintf(buff, 4094, format, args);
    va_end(args);

    sLogCallback(buff, UWK_LOG_VERBOSE);

}
