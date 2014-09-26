
/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "uwk_gpusurface.h"

#include <IOSurface/IOSurface.h>
#include <OpenGL/gl.h>

namespace UWK
{

class GpuSurfaceMac : public GpuSurface
{
    IOSurfaceRef surfaceRef_;

    // IOSurfaceID is defined as uint32_t
    uintptr_t surfaceID_;

    uint32_t maxWidth_;
    uint32_t maxHeight_;

public:

    GpuSurfaceMac(uint32_t maxWidth, uint32_t maxHeight);

    ~GpuSurfaceMac();

    void UpdateTexture(const void* image_buffer);

    void UpdateTexture(const void* image_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                       uint32_t rowLength, uint32_t skipPixels, uint32_t skipRows);

    uintptr_t GetSurfaceID() { return surfaceID_; }

};

}
