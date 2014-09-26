/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <stdio.h>
#include "uwk_error.h"

UWKErrorCallback UWKError::sErrorCallback = NULL;

void UWKError::SetErrorCallback(UWKErrorCallback callback)
{
    sErrorCallback = callback;
}

void UWKError::FatalError(const char *format, ...)
{

    if (!sErrorCallback)
    {
        // we probably need to do something here, if this is ever undefined?
        return;
    }

    char buff[4096];
    va_list args;

    va_start(args, format);
    vsnprintf(buff, 4094, format, args);
    va_end(args);

    sErrorCallback(buff, true);

}

