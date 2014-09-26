/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <stdio.h>
#include "UWKCommon/uwk_log.h"
#include "UWKCommon/uwk_config.h"
#include "uwk_gpusurface_sharedmemory.h"

namespace UWK
{

uint32_t GpuSurfaceSharedMemory::sSurfaceIdCounter = 0;

GpuSurfaceSharedMemory::GpuSurfaceSharedMemory(uint32_t maxWidth, uint32_t maxHeight) :
    handle_(NULL),
    maxWidth_(maxWidth),
    maxHeight_(maxHeight)
{
    surfaceId_ = sSurfaceIdCounter++;

    char name[1024];
    std::string prefix;
    UWKConfig::GetSharedMemoryPrefix(prefix);
    if (prefix.length())
        _snprintf(name, 1024, "%suwk_gpu_surface_%u_%u", prefix.c_str(), surfaceId_, UWKConfig::GetServerID());
    else
        _snprintf(name, 1024, "uwk_gpu_surface_%u_%u", surfaceId_, UWKConfig::GetServerID());

    int len = strlen(name)+1;
    wchar_t *wText = new wchar_t[len];
    memset(wText,0,len);
    MultiByteToWideChar(  CP_ACP, NULL,name, -1, wText,len );

    handle_ = CreateFileMapping( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, maxWidth_ * maxHeight_ * 4, wText );

    delete [] wText;

    if (!handle_)
    {
        UWKLog::LogVerbose("Error creating file mapping");
    }

}

GpuSurfaceSharedMemory::~GpuSurfaceSharedMemory()
{
    if (handle_)
    {
        CloseHandle(handle_);
    }
}

void GpuSurfaceSharedMemory::UpdateTexture(const void* image_buffer)
{

    void* memory = MapViewOfFile( handle_ , FILE_MAP_WRITE, 0, 0, maxWidth_ * maxHeight_ * 4 );

    if (!memory)
        return;

    unsigned char* out = (unsigned char*) memory;
    unsigned char* in = (unsigned char*) image_buffer;

    uint32_t row = maxWidth_ * 4;

    in += (maxHeight_ - 1) * row;

    for (uint32_t y = 0; y < maxHeight_; y++)
    {
        memcpy(out, in, row);
        in -= row;
        out += row;
    }

    UnmapViewOfFile(memory);

}

void GpuSurfaceSharedMemory::UpdateTexture(const void* image_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                                           uint32_t rowLength, uint32_t skipPixels, uint32_t skipRows)
{

}

}


