/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/


#include "uwk_gpusurface_d3d9.h"

namespace UWK
{

GpuSurfaceD3D9::GpuSurfaceD3D9(uint32_t maxWidth, uint32_t maxHeight) :
    GpuSurfaceSharedMemory(maxWidth, maxHeight)
{

}

}
