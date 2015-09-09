/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#include "uwk_process_utils.h"

#ifdef _MSC_VER

#include "Poco/Path.h"

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <Psapi.h>

#else

#include <libproc.h>
#include <errno.h>

#endif

#ifdef _MSC_VER
bool UWKProcessUtils::GetExecutablePath(const UWKProcessCommon::PID& pid, std::string& path)
{
    HANDLE processHandle = NULL;
    WCHAR filename[MAX_PATH];

    processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (processHandle != NULL)
    {
        unsigned int numChars = GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH);
        if ( numChars == 0)
        {
            CloseHandle(processHandle);
            return false;
        }
        else
        {
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, filename, numChars, NULL, 0, NULL, NULL);
            std::string strTo( size_needed, 0 );
            WideCharToMultiByte(CP_UTF8, 0, filename, numChars, &strTo[0], size_needed, NULL, NULL);
            CloseHandle(processHandle);
            path = strTo;
            return true;
        }

    }

    return false;
}

void UWKProcessUtils::EnsureExecutable(const std::string& path)
{

}

#else

#include <sys/stat.h>

void UWKProcessUtils::EnsureExecutable(const std::string& path)
{
    struct stat buf;

    stat(path.c_str(), &buf);

    bool set  = false;

    uint32_t permissions[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IXGRP, S_IROTH, S_IXOTH, 0xFFFF};

    int x = 0;
    while(permissions[x] != 0xFFFF)
    {
        if (!(buf.st_mode & permissions[x]))
        {
            buf.st_mode |= permissions[x];
            set = true;
        }

        x++;
    }

    bool error = false;

    if (set)
    {
        chmod(path.c_str(), buf.st_mode);

        stat(path.c_str(), &buf);

        x = 0;
        while(permissions[x] != 0xFFFF)
        {
            if (!(buf.st_mode & permissions[x]))
            {
                error = true;
            }

            x++;
        }

    }

    if (error)
        UWKLog::LogVerbose("Unable to set executable permissions: %s", path.c_str());

}

bool UWKProcessUtils::GetExecutablePath(const UWKProcessCommon::PID& pid, std::string& path)
{
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];

    path = "";

    int ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));

    if ( ret <= 0 )
    {
        return false;
    }

    path = pathbuf;

    return true;
}
#endif

bool UWKProcessUtils::CompareExecutablePaths(const std::string& path1, const std::string& path2)
{

    if (path1.length() != path2.length())
        return false;

    for (size_t i = 0; i < path1.length(); i++)
    {
        if ( (path1[i] == '/' || path1[i] == '\\') && (path2[i] == '/' || path2[i] == '\\'))
            continue;

        if ( tolower(path1[i]) != tolower(path2[i]) )
            return false;
    }

    return true;

}
