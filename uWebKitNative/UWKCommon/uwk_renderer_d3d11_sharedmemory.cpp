/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#include "uwk_renderer_d3d11_sharedmemory.h"
#include "uwk_browser.h"

ID3D11Device* UWKRendererD3D11SharedMemory::sD3D11Device_ = NULL;

UWKRendererD3D11SharedMemory::UWKRendererD3D11SharedMemory(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr) :
    UWKRendererSharedMemory(maxWidth, maxHeight, nativeTexturePtr),
    texture_(NULL)
{
}


void UWKRendererD3D11SharedMemory::SetDevice(ID3D11Device *device)
{
    sD3D11Device_ = device;
}

void UWKRendererD3D11SharedMemory::Initialize(const UWKMessage &gpuSurfaceInfo)
{
    if (!sD3D11Device_)
        return; // this is an error

    UWKRendererSharedMemory::Initialize(gpuSurfaceInfo);

    if (!valid_)
        return;

    valid_ = false;

    // note that Unity does say "IDirect3DBaseTexture9*"
    texture_ = (ID3D11Texture2D*) nativeTexturePtr_;

    // mark as valid
    valid_ = true;

}


UWKRendererD3D11SharedMemory::~UWKRendererD3D11SharedMemory()
{
    if (valid_)
    {

    }
}


void UWKRendererD3D11SharedMemory::UpdateTexture()
{
    if (!valid_)
        return;

    ID3D11DeviceContext* ctx = NULL;
    sD3D11Device_->GetImmediateContext (&ctx);

    D3D11_TEXTURE2D_DESC desc;
    texture_->GetDesc (&desc);

    void* memory = LockMemory();

    ctx->UpdateSubresource (texture_, 0, NULL, memory, desc.Width*4, 0);

    UnlockMemory(memory);

    ctx->Release();
}





