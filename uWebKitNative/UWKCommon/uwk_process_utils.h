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

class UWKProcessUtils
{

public:

    // compares paths ignoring differences in '/' and '\'
    static bool CompareExecutablePaths(const std::string& path1, const std::string& path2);

    static bool GetExecutablePath(const UWKProcessCommon::PID& pid, std::string& path);

    static void EnsureExecutable(const std::string& path);
};


