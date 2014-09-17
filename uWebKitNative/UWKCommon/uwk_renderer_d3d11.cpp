/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#include <windows.h>
#include "uwk_renderer_d3d11.h"
#include "uwk_browser.h"

ID3D11Device* UWKRendererD3D11::sD3D11Device_ = NULL;

UWKRendererD3D11::UWKRendererD3D11(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr) :
    UWKRenderer(maxWidth, maxHeight, nativeTexturePtr),
    texture_(NULL), sharedTexture_(NULL), sharedHandle_(NULL), valid_(false)
{

}

void UWKRendererD3D11::SetDevice(ID3D11Device *device)
{
    sD3D11Device_ = device;
}

bool UWKRendererD3D11::SetupSharedTexture()
{
    if (!sD3D11Device_ || !sharedHandle_ || !nativeTexturePtr_)
        return false; // this is an error

    // note that Unity does say "IDirect3DBaseTexture9*"
    texture_ = (ID3D11Texture2D*) nativeTexturePtr_;

    HRESULT hr = sD3D11Device_->OpenSharedResource(sharedHandle_, __uuidof(ID3D11Texture2D), (LPVOID*)&sharedTexture_);

    if (hr != S_OK)
    {
        UWKLog::LogVerbose("Unabled to OpenSharedResource on Direct3D11 texture");
        sharedHandle_ = NULL;
        return false;
    }

    /*
    D3D11_TEXTURE2D_DESC texDesc;
    texture_->GetDesc(&texDesc);

    D3D11_TEXTURE2D_DESC sharedDesc;
    sharedTexture_->GetDesc(&sharedDesc);

    if (texDesc.Format != sharedDesc.Format)
    {
        UWKLog::LogVerbose("texDesc.Format != sharedDesc.Format");
    }

    if (texDesc.Width != sharedDesc.Width)
    {
        UWKLog::LogVerbose("texDesc.Width != sharedDesc.Width");
    }

    if (texDesc.Height != sharedDesc.Height)
    {
        UWKLog::LogVerbose("texDesc.Height != sharedDesc.Height");
    }

    if (texDesc.MipLevels != sharedDesc.MipLevels)
    {
        UWKLog::LogVerbose("texDesc.MipLevels != sharedDesc.MipLevels");
    }

    if (texDesc.BindFlags != sharedDesc.BindFlags)
    {
        UWKLog::LogVerbose("texDesc.BindFlags != sharedDesc.BindFlags");
    }

    if (texDesc.Usage != sharedDesc.Usage)
    {
        UWKLog::LogVerbose("texDesc.Usage != sharedDesc.Usage");
    }

    UWKLog::LogVerbose("Opened shared Direct3D11 texture");

    */

    // mark as valid
    valid_ = true;

    return true;

}

void UWKRendererD3D11::Initialize(const UWKMessage &gpuSurfaceInfo)
{
    valid_ = false;


    sharedHandle_ = (HANDLE) ParseGPUSurface(gpuSurfaceInfo);

}


UWKRendererD3D11::~UWKRendererD3D11()
{
    if (valid_)
    {

    }
}


void UWKRendererD3D11::UpdateTexture()
{
    if (!valid_ && sharedHandle_)
    {
        SetupSharedTexture();
    }

    if (!valid_ || !texture_ || !sharedTexture_)
        return;

    ID3D11DeviceContext* ctx = NULL;
    sD3D11Device_->GetImmediateContext (&ctx);

    ctx->CopyResource(texture_, sharedTexture_);

    ctx->Release();
}





