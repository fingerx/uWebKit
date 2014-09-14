/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_config.h"
#include "uwk_renderer.h"
#include "uwk_browser.h"
#include "uwk_server.h"

#ifdef _MSC_VER

#include "uwk_renderer_d3d9.h"
#include "uwk_renderer_d3d11.h"
#include "uwk_renderer_d3d11_sharedmemory.h"


#else

#include "uwk_renderer_gl.h"

#endif

std::vector<UWKRenderer*> UWKRenderer::sRendererDeleteQueue_;

UWKRenderer::UWKRenderer(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr) :
    maxWidth_(maxWidth),
    maxHeight_(maxHeight),
    browser_(NULL),
    nativeTexturePtr_(nativeTexturePtr)

{

}

UWKRenderer::~UWKRenderer()
{

}


void UWKRenderer::Initialize()
{
    if (sRendererDeleteQueue_.size())
    {
        UWKLog::LogVerbose("Warning: renderer delete queue not empty, clearing");
        sRendererDeleteQueue_.clear();
    }

}

UWKRenderer* UWKRenderer::CreateRenderer(UWKBrowser* browser, void* nativeTexturePtr)
{
#ifdef _MSC_VER
    UWKRenderer* renderer = NULL;
    if (UWKConfig::IsDirect3D9())
        renderer = new UWKRendererD3D9(browser->GetMaxWidth(), browser->GetMaxHeight(), nativeTexturePtr);
    else
    {
        renderer = new UWKRendererD3D11(browser->GetMaxWidth(), browser->GetMaxHeight(), nativeTexturePtr);

        // slower, doesn't use share GPU resource
        //UWKRendererD3D11SharedMemory

    }

#else
    UWKRenderer* renderer = new UWKRendererGL(browser->GetMaxWidth(), browser->GetMaxHeight(), nativeTexturePtr);
#endif

    browser->SetRenderer(renderer);
    renderer->browser_ = browser;
    return browser->GetRenderer();

}

void UWKRenderer::Shutdown()
{

}

void UWKRenderer::QueueRenderDelete()
{
    UWKServer::LockRender();

    sRendererDeleteQueue_.push_back(this);

    UWKServer::UnlockRender();
}

// only call from render thread, if any
void UWKRenderer::DeleteRenderers()
{
    for (size_t i = 0; i < sRendererDeleteQueue_.size(); i++)
    {
        UWKRenderer* renderer = sRendererDeleteQueue_.at(i);
        delete renderer;
    }

    sRendererDeleteQueue_.clear();

}

uintptr_t UWKRenderer::ParseGPUSurface(const UWKMessage& gpuSurfaceInfo)
{
    uintptr_t gpuSurface;
    void* data;
    uint32_t size;

    UWKMessageQueue::GetData(gpuSurfaceInfo, 0, &data, size);

    if (size != sizeof(uintptr_t))
        return 0;

    gpuSurface = *((uintptr_t*) data);

    return gpuSurface;

}


