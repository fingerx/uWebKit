/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <stdio.h>
#include "uwk_config.h"
#include "uwk_renderer_sharedmemory.h"

UWKRendererSharedMemory::UWKRendererSharedMemory(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr):
    UWKRenderer(maxWidth, maxHeight, nativeTexturePtr),
    valid_(false), handle_(NULL)
{

}

UWKRendererSharedMemory::~UWKRendererSharedMemory()
{
    if (handle_)
    {
        CloseHandle(handle_);
    }

}

void UWKRendererSharedMemory::Initialize(const UWKMessage& gpuSurfaceInfo)
{
    surfaceId_ = (uint32_t) ParseGPUSurface(gpuSurfaceInfo);

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
    handle_ = OpenFileMapping(FILE_MAP_READ, false, wText);

    delete [] wText;

    valid_ = true;
}


void* UWKRendererSharedMemory::LockMemory()
{
    if (!valid_)
        return NULL;

    void* memory = MapViewOfFile( handle_ , FILE_MAP_READ, 0, 0, maxWidth_ * maxHeight_ * 4 );

    return memory;
}

void UWKRendererSharedMemory::UnlockMemory(void* memory)
{
    UnmapViewOfFile(memory);
}


void UWKRendererSharedMemory::BlitToTexture(void* textureMemory)
{
    if (!valid_)
        return;

    void* memory = MapViewOfFile( handle_ , FILE_MAP_READ, 0, 0, maxWidth_ * maxHeight_ * 4 );

    if (!memory)
        return;

    memcpy(textureMemory, memory, maxWidth_ * maxHeight_ * 4);

    UnmapViewOfFile(memory);
}





