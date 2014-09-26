/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_error.h"
#include "uwk_message.h"
#include "uwk_renderer.h"
#include "uwk_browser.h"
#include "uwk_server.h"

UWKBrowser::UWKBrowser(uint32_t id, uint32_t maxWidth, uint32_t maxHeight):
    renderer_(NULL),
    nativeTexturePtr_(NULL),
    visible_(true)
{
    id_ = id;
    maxWidth_ = maxWidth;
    maxHeight_ = maxHeight;
}

UWKBrowser::~UWKBrowser()
{
    if (renderer_)
    {
        if (!UWKServer::UsingRenderThread())
        {
            delete renderer_;
        }
        else
        {
            renderer_->QueueRenderDelete();
        }
    }
}


void UWKBrowser::UpdateTexture()
{
    if (!renderer_)
        return;

    if (!visible_)
        return;

    renderer_->UpdateTexture();
}

void UWKBrowser::Restart()
{
    if (!nativeTexturePtr_)
    {
        UWKError::FatalError("Unable to restart browser without native texture ptr");
        return;
    }

    DestroyRenderer();

    CreateRenderer(flags_);

    UWKMessage msg;
    msg.type = UMSG_VIEW_CREATE;
    msg.browserID = id_;
    msg.iParams[0] = maxWidth_;
    msg.iParams[1] = maxHeight_;

    UWKMessageQueue::SetString(msg, 0, currentURL_.c_str());

    UWKMessageQueue::Write(msg);

}

void UWKBrowser::DestroyRenderer()
{
    if (!renderer_)
        return;

    if (!UWKServer::UsingRenderThread())
    {
        delete renderer_;
    }
    else
    {
        renderer_->QueueRenderDelete();
    }

    renderer_ = NULL;

}

void UWKBrowser::CreateRenderer(uint32_t flags)
{
    flags_ = flags;
    UWKRenderer::CreateRenderer(this, flags, nativeTexturePtr_);
}

void UWKBrowser::SetNativeTexture(void* nativeTexturePtr)
{
    nativeTexturePtr_ = nativeTexturePtr;
}

