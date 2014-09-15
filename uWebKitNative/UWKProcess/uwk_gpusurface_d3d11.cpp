/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#include "uwk_gpusurface_d3d11.h"
#include "UWKCommon/uwk_log.h"

namespace UWK
{

ID3D11Device* GpuSurfaceD3D11::d3dDevice_ = NULL;
ID3D11DeviceContext* GpuSurfaceD3D11::d3dContext_ = NULL;

// dynamic link D3D11, so we can still run without it
typedef HRESULT(WINAPI *LPD3D11CREATEDEVICE)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT32, D3D_FEATURE_LEVEL *, UINT, UINT32, ID3D11Device **, D3D_FEATURE_LEVEL *, ID3D11DeviceContext **);
static HMODULE sHModD3D11 = NULL;
static LPD3D11CREATEDEVICE sFnPtrD3D11CreateDevice = NULL;

GpuSurfaceD3D11::GpuSurfaceD3D11(uint32_t maxWidth, uint32_t maxHeight)
{
    HRESULT hr;

    maxWidth_ = maxWidth;
    maxHeight_ = maxHeight;
    flippedImage_ = (uint8_t*) malloc(maxWidth_ * maxHeight_ * 4);
    texture2d_ = NULL;

    if (!d3dDevice_)
    {
        if (sHModD3D11 == NULL)
        {
            sHModD3D11 = LoadLibraryA("d3d11.dll");
            if(!sHModD3D11)
            {
                UWKLog::LogVerbose("Error loading d3d11.dll in UWKProcess");
                return;
            }

            sFnPtrD3D11CreateDevice = (LPD3D11CREATEDEVICE)GetProcAddress(sHModD3D11, "D3D11CreateDevice");
            if (!sFnPtrD3D11CreateDevice)
            {
                UWKLog::LogVerbose("Error getting D3D11CreateDevice function in d3d11.dll");
                return;
            }
        }


        UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        D3D_FEATURE_LEVEL featureLevel;
        hr = sFnPtrD3D11CreateDevice(
                    NULL,
                    D3D_DRIVER_TYPE_HARDWARE,
                    NULL,
                    createDeviceFlags,
                    0, 0,
                    D3D11_SDK_VERSION,
                    &d3dDevice_,
                    &featureLevel,
                    &d3dContext_);

        if (hr != S_OK)
        {
            UWKLog::LogVerbose("Error Creating UWKProcess Direct3D11 device, need shared memory fallback");
            return;
        }
    }

    D3D11_TEXTURE2D_DESC td;

    memset(&td, 0, sizeof(td));

    td.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    td.Width = maxWidth_;
    td.Height = maxHeight_;
    td.MipLevels = td.ArraySize = 1;

    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags =  D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    td.CPUAccessFlags = 0;

    hr = d3dDevice_->CreateTexture2D(&td, 0, &texture2d_);

    if (hr != S_OK)
    {
        texture2d_ = NULL;
        UWKLog::LogVerbose("Error Creating UWKProcess Direct3D11 texture");
        return;
    }

    IDXGIResource *dxgiResource = 0;
    hr = texture2d_->QueryInterface(__uuidof(IDXGIResource), reinterpret_cast<void **>(&dxgiResource));
    if (hr != S_OK)
    {
        UWKLog::LogVerbose("Error Getting IDXGIResource inteface of Direct3D11 texture");
        return;
    }


    hr = dxgiResource->GetSharedHandle(&surfaceID_);

    if (hr != S_OK)
    {
        UWKLog::LogVerbose("Error Getting UWKProcess Direct3D11 texture shared handle");
        return;
    }

    dxgiResource->Release();

    void* clear = malloc(maxWidth * maxHeight * 4);
    memset(clear, 255, maxWidth * maxHeight * 4);
    UpdateTexture(clear);
    free(clear);


}

void GpuSurfaceD3D11::UpdateTexture(const void* image_buffer)
{
    if (!d3dContext_ || !texture2d_)
        return;

    // having to flip the image is unfortunate
    // note that the d3d9 shared memory also has to
    // flip
    uint8_t* out = (unsigned char*) flippedImage_;
    uint8_t* in = (unsigned char*) image_buffer;

    uint32_t row = maxWidth_ * 4;

    in += (maxHeight_ - 1) * row;

    for (uint32_t y = 0; y < maxHeight_; y++)
    {
        memcpy(out, in, row);
        in -= row;
        out += row;
    }

    d3dContext_->UpdateSubresource (texture2d_, 0, NULL, flippedImage_, maxWidth_ * 4, 0);
    d3dContext_->Flush();

    return;
}

void GpuSurfaceD3D11::UpdateTexture(const void* image_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                                    uint32_t rowLength, uint32_t skipPixels, uint32_t skipRows)
{
    //UpdateTexture(image_buffer);
}

GpuSurfaceD3D11::~GpuSurfaceD3D11()
{

    if (flippedImage_)
        free(flippedImage_);

}


}
