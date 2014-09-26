/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "uwk_message.h"
#include "uwk_renderer.h"

#include <windows.h>

class UWKRendererSharedMemory : public UWKRenderer
{
protected:

    bool valid_;
    uint32_t surfaceId_;
    HANDLE handle_;

public:

    UWKRendererSharedMemory(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr);
    virtual ~UWKRendererSharedMemory();

    virtual void Initialize(const UWKMessage& gpuSurfaceInfo);
    virtual bool IsValid() { return valid_; }

    void BlitToTexture(void* textureMemory);

    void* LockMemory();

    void UnlockMemory(void *memory);

    virtual void UpdateTexture() = 0;

};
