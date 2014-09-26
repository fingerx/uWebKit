/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "UWKCommon/uwk_log.h"
#include "uwk_gpusurface_mac.h"
#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>

namespace UWK
{

void GpuSurfaceMac::UpdateTexture(const void* image_buffer)
{
    uint32_t seed;

    if (IOSurfaceLock(surfaceRef_, kIOSurfaceLockAvoidSync, &seed) != kIOReturnSuccess)
    {
        return;
    }

    void* sdata = IOSurfaceGetBaseAddress(surfaceRef_);
    memcpy(sdata, image_buffer, maxWidth_ * maxHeight_ * 4);
    IOSurfaceUnlock(surfaceRef_, kIOSurfaceLockAvoidSync, &seed);
}

void GpuSurfaceMac::UpdateTexture(const void* image_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                                     uint32_t rowLength, uint32_t skipPixels, uint32_t skipRows)
{


    /*
    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_pixels);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, skip_rows);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_BGRA,
                    GL_UNSIGNED_INT_8_8_8_8_REV, buffer);
                    */

    //UWKLog::LogVerbose("UpdateTexture %i %i %i %i", x, y, width, height);

    uint32_t seed;

    IOSurfaceLock(surfaceRef_, 0, &seed);

    uint32_t* pixels = (uint32_t*) IOSurfaceGetBaseAddress(surfaceRef_);

    // get starting pixel

    uint32_t* srcPixels = (uint32_t*) image_buffer;

    srcPixels = srcPixels + (skipRows * rowLength) + skipPixels;
    pixels = pixels + (y * maxWidth_) + x;

    for (uint32_t i = 0; i < height; i++)
    {
        memcpy(pixels, srcPixels, sizeof(uint32_t) * width);
        pixels += maxWidth_;
        srcPixels += rowLength;
    }

    IOSurfaceUnlock(surfaceRef_, 0, &seed);

}

GpuSurfaceMac::~GpuSurfaceMac()
{


    if (surfaceRef_)
        IOSurfaceDecrementUseCount(surfaceRef_);



}

GpuSurfaceMac::GpuSurfaceMac(uint32_t maxWidth, uint32_t maxHeight)
{
    maxWidth_ = maxWidth;
    maxHeight_ = maxHeight;

    surfaceRef_ = IOSurfaceCreate((CFDictionaryRef)[NSDictionary dictionaryWithObjectsAndKeys:
    [NSNumber numberWithInt:maxWidth],  (id)kIOSurfaceWidth,
    [NSNumber numberWithInt:maxHeight], (id)kIOSurfaceHeight,
    [NSNumber numberWithInt:4],      (id)kIOSurfaceBytesPerElement,
    [NSNumber numberWithBool:YES],   (id)kIOSurfaceIsGlobal, nil]);

    if (surfaceRef_)
        IOSurfaceIncrementUseCount(surfaceRef_);

    void* clear = malloc(maxWidth * maxHeight * 4);
    memset(clear, 0, maxWidth * maxHeight * 4);
    UpdateTexture(clear);
    free(clear);


    surfaceID_ = IOSurfaceGetID(surfaceRef_);

}

}

