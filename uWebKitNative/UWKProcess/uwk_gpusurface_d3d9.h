/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "uwk_gpusurface_sharedmemory.h"

namespace UWK
{

// we can't count on Direct3D9Ex being used
class GpuSurfaceD3D9 : public GpuSurfaceSharedMemory
{

public:

    GpuSurfaceD3D9(uint32_t maxWidth, uint32_t maxHeight);
};

}
