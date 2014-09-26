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

typedef void (*UWKErrorCallback)(const char* string, bool fatal);

class UWKError
{

    static UWKErrorCallback sErrorCallback;

public:

    static void SetErrorCallback(UWKErrorCallback callback);

    static void FatalError(const char *format, ...);

};
