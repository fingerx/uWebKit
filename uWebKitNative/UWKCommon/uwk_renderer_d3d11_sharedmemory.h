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

#include <d3d11.h>

class UWKRendererD3D11SharedMemory : public UWKRendererSharedMemory
{

    ID3D11Texture2D* texture_;
    static ID3D11Device* sD3D11Device_;

public:

    UWKRendererD3D11SharedMemory(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr);

    virtual ~UWKRendererD3D11SharedMemory();

    static void SetDevice(ID3D11Device* device);

    void Initialize(const UWKMessage& gpuSurfaceInfo);
    bool IsValid() { return valid_; }
    void UpdateTexture();



};
