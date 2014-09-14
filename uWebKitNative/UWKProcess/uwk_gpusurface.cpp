/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "UWKCommon/uwk_config.h"
#include "uwk_gpusurface.h"

#ifdef _MSC_VER
#include "uwk_gpusurface_d3d9.h"
#include "uwk_gpusurface_d3d11.h"
#else
#include "uwk_gpusurface_mac.h"
#endif

namespace UWK
{

GpuSurface* GpuSurface::Create(int maxWidth, int maxHeight)
{
#ifdef _MSC_VER
    if (UWKConfig::IsDirect3D9())
        return new GpuSurfaceD3D9(maxWidth, maxHeight);
    else
        return new GpuSurfaceD3D11(maxWidth, maxHeight);
#else
    return new GpuSurfaceMac(maxWidth, maxHeight);
#endif
}

}
