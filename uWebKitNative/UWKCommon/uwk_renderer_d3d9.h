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
#include "uwk_renderer_sharedmemory.h"

#include <d3d9.h>

class UWKRendererD3D9 : public UWKRendererSharedMemory
{

    IDirect3DTexture9* texture_;
    static IDirect3DDevice9* sD3D9Device_;

public:

    UWKRendererD3D9(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr);

    virtual ~UWKRendererD3D9();

    static void SetDevice(IDirect3DDevice9* device);

    void Initialize(const UWKMessage& gpuSurfaceInfo);
    bool IsValid() { return valid_; }
    void UpdateTexture();



};
