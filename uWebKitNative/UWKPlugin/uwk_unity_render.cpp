/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_unity_render.h"

#if SUPPORT_D3D9
#include "UWKCommon/uwk_renderer_d3d9.h"
#include <d3d9.h>
#endif
#if SUPPORT_D3D11
#include "UWKCommon/uwk_renderer_d3d11.h"
#include "UWKCommon/uwk_renderer_d3d11_sharedmemory.h"
#include <d3d11.h>
#endif

#if SUPPORT_OPENGL
#include "UWKCommon/uwk_renderer_gl.h"
#endif

static int g_DeviceType = -1;

// Actual setup/teardown functions defined below
#if SUPPORT_D3D9
static void SetGraphicsDeviceD3D9 (IDirect3DDevice9* device, GfxDeviceEventType eventType);
#endif
#if SUPPORT_D3D11
static void SetGraphicsDeviceD3D11 (ID3D11Device* device, GfxDeviceEventType eventType);
#endif

extern "C"
{

void EXPORT_API UnitySetGraphicsDevice (void* device, int deviceType, int eventType)
{
    // Set device type to -1, i.e. "not recognized by our plugin"
    g_DeviceType = -1;

#if SUPPORT_OPENGL

    if (deviceType == kUnityGfxRendererOpenGLCore)
    {
        UWKRendererGL::SetGLCore(true);
    }
    else if (deviceType == kUnityGfxRendererOpenGL)
    {
        UWKRendererGL::SetGLCore(false);
    }

#endif

#if SUPPORT_D3D9
    // D3D9 device, remember device pointer and device type.
    // The pointer we get is IDirect3DDevice9.
    if (deviceType == kGfxRendererD3D9)
    {
        g_DeviceType = deviceType;
        SetGraphicsDeviceD3D9 ((IDirect3DDevice9*)device, (GfxDeviceEventType)eventType);
    }
#endif

#if SUPPORT_D3D11
    // D3D11 device, remember device pointer and device type.
    // The pointer we get is ID3D11Device.
    if (deviceType == kGfxRendererD3D11)
    {
        g_DeviceType = deviceType;
        SetGraphicsDeviceD3D11 ((ID3D11Device*)device, (GfxDeviceEventType)eventType);
    }
#endif

}

}

#if SUPPORT_D3D9

static void SetGraphicsDeviceD3D9 (IDirect3DDevice9* device, GfxDeviceEventType eventType)
{
    UWKRendererD3D9::SetDevice(device);
}
#endif // #if SUPPORT_D3D9

#if SUPPORT_D3D11
static void SetGraphicsDeviceD3D11 (ID3D11Device* device, GfxDeviceEventType eventType)
{

    UWKRendererD3D11::SetDevice(device);
    UWKRendererD3D11SharedMemory::SetDevice(device);
}
#endif // #if SUPPORT_D3D11



