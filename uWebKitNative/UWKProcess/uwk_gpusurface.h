/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "stdint.h"
#include "stdlib.h"

namespace UWK
{

class GpuSurface
{

public:

    virtual ~GpuSurface() {}

    virtual void UpdateTexture(const void* image_buffer)  = 0;

    virtual void UpdateTexture(const void* image_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                               uint32_t rowLength, uint32_t skipPixels, uint32_t skipRows) = 0;

    virtual uintptr_t GetSurfaceID() = 0;

    static GpuSurface* Create(int maxWidth, int maxHeight);

};

}
