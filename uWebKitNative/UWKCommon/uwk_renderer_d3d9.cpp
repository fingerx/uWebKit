/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_renderer_d3d9.h"
#include "uwk_browser.h"

IDirect3DDevice9* UWKRendererD3D9::sD3D9Device_ = NULL;

UWKRendererD3D9::UWKRendererD3D9(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr) :
    UWKRendererSharedMemory(maxWidth, maxHeight, nativeTexturePtr),
    texture_(NULL)
{
}


void UWKRendererD3D9::SetDevice(IDirect3DDevice9* device)
{
    sD3D9Device_ = device;
}

void UWKRendererD3D9::Initialize(const UWKMessage &gpuSurfaceInfo)
{
    if (!sD3D9Device_)
        return; // this is an error

    UWKRendererSharedMemory::Initialize(gpuSurfaceInfo);

    if (!valid_)
        return;

    valid_ = false;

    // note that Unity does say "IDirect3DBaseTexture9*"
    texture_ = (IDirect3DTexture9*) nativeTexturePtr_;

    // Unity isn't using a IDirect3DDevice9Ex device, so we can't use a shared texture
    //IDirect3DDevice9Ex *D3DDev9Ex = NULL;
    //bool IsD3DDev9Ex = SUCCEEDED(sD3D9Device_->QueryInterface(__uuidof(IDirect3DDevice9Ex), (void **)&D3DDev9Ex)) && D3DDev9Ex != NULL;

    // mark as valid
    valid_ = true;

}


UWKRendererD3D9::~UWKRendererD3D9()
{
    if (valid_)
    {

    }
}


void UWKRendererD3D9::UpdateTexture()
{
    if (!valid_)
        return;

    D3DLOCKED_RECT rect;
    if (texture_->LockRect(0, &rect, NULL, D3DLOCK_DISCARD) != D3D_OK)
        return;

    BlitToTexture(rect.pBits);

    texture_->UnlockRect(0);

}





