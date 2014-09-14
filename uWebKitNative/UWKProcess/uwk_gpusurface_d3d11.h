/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#pragma once

#include <D3D11.h>
#include "uwk_gpusurface_sharedmemory.h"

namespace UWK
{

class GpuSurfaceD3D11 : public GpuSurface
{
    uint32_t maxWidth_;
    uint32_t maxHeight_;
    //uint32_t surfaceID_;

    ID3D11Texture2D* texture2d_;

    static ID3D11Device* d3dDevice_;
    static ID3D11DeviceContext* d3dContext_;

    // void*, won't work for 64 bit
    HANDLE surfaceID_;

public:

    GpuSurfaceD3D11(uint32_t maxWidth, uint32_t maxHeight);
    ~GpuSurfaceD3D11();

    void UpdateTexture(const void* image_buffer);

    void UpdateTexture(const void* image_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                       uint32_t rowLength, uint32_t skipPixels, uint32_t skipRows);

    // won't work for 64 bit
    uint32_t GetSurfaceID() { return (uint32_t) surfaceID_; }

};

}
