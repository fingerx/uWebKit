/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_server.h"
#include "uwk_message.h"
#include "uwk_process_server.h"

UWKServer* UWKServer::instance_ = NULL;

uint32_t UWKServer::sBrowserIDCounter_ = 1;

void UWKServer::PumpMessages()
{
    LockRender();

    if (UWKProcessServer::Instance()->Update())
    {
        // problem with render client
        Restart();
    }

    UWKMessage msg;

    if (UWKMessageQueue::Read(msg))
    {
        std::string str;

        if (msg.type == UMSG_GPUSURFACE_INFO)
        {
            UWKBrowser* browser = GetBrowser(msg.browserID);
            if (browser)
            {
                browser->CreateRenderer((uint32_t)msg.iParams[0]);
                browser->GetRenderer()->Initialize(msg);
            }
        }
        else if (msg.type == UMSG_LOG)
        {

            UWKMessageQueue::GetString(msg, 0, str);
            UWKLog::LogVerbose(str.c_str());
        }
        else if (msg.type == UMSG_ERROR)
        {
            UWKMessageQueue::GetString(msg, 0, str);
            UWKError::FatalError(str.c_str());
        }
        else if (msg.type == UMSG_VIEW_URLCHANGED)
        {
            UWKBrowser* browser = GetBrowser(msg.browserID);
            if (browser)
            {
                UWKMessageQueue::GetString(msg, 0, str);
                browser->SetCurrentURL(str);
            }
        }

        if (processCB_)
            processCB_(&msg);

        UWKMessageQueue::Dispose(msg);

    }

    UnlockRender();
}

UWKBrowser* UWKServer::CreateBrowser(uint32_t maxWidth, uint32_t maxHeight, std::string url, void* nativeTexturePtr)
{
    LockRender();

    uint32_t id = sBrowserIDCounter_++;

    UWKBrowser* browser = new UWKBrowser(id, maxWidth, maxHeight);

    browser->SetNativeTexture(nativeTexturePtr);

    browserMap_.insert(std::pair<uint32_t, UWKBrowser*>(id, browser));

    UWKMessage msg;
    msg.type = UMSG_VIEW_CREATE;
    msg.browserID = id;
    msg.iParams[0] = maxWidth;
    msg.iParams[1] = maxHeight;

    UWKMessageQueue::SetString(msg, 0, url.c_str());

    UWKMessageQueue::Write(msg);

    UnlockRender();

    return browser;
}

void UWKServer::DestroyBrowser(uint32_t id)
{
    LockRender();

    std::map<uint32_t, UWKBrowser*>::iterator itr;

    itr = browserMap_.find(id);

    if (itr == browserMap_.end())
    {
        // error
        return;
    }

    UWKBrowser* browser = itr->second;

    browserMap_.erase(itr);

    delete browser;

    UnlockRender();
}

void UWKServer::Restart()
{    
    // dump all messages and recreate queues
    UWKMessageQueue::RecreateQueues();

    std::vector<std::string> args;

    // destroy current renderers
    for (std::map<uint32_t, UWKBrowser*>::iterator itr = browserMap_.begin(); itr != browserMap_.end(); itr++)
    {
        itr->second->Restart();
    }

    if (!UWKProcessServer::Instance()->SpawnClient(args))
    {
        UWKError::FatalError("Could not spawn web rendering process");
        return;
    }

}

UWKBrowser* UWKServer::GetBrowser(uint32_t id)
{
    std::map<uint32_t, UWKBrowser*>::iterator itr = browserMap_.find(id);

    if (itr == browserMap_.end())
    {
        return NULL;
    }

    return itr->second;

}

bool UWKServer::Initialize(UWKProcessMessage processCB, bool useRenderThread)
{
    UWKProcessServer::Initialize();

    instance_ = new UWKServer();
    instance_->processCB_ = processCB;

    if (useRenderThread)
        instance_->renderMutex_ = new Poco::Mutex();

    // initialize subsystems
    UWKMessageQueue::Initialize(true);
    UWKRenderer::Initialize();

    std::vector<std::string> args;

    if (!UWKProcessServer::Instance()->SpawnClient(args))
    {
        UWKError::FatalError("Could not spawn web rendering process");
        return false;
    }

    return true;
}

void UWKServer::Shutdown()
{

    UWKProcessServer::Shutdown();

    if (!instance_)
        return;

    // cleanup browsers

    for (std::map<uint32_t, UWKBrowser*>::iterator itr = instance_->browserMap_.begin(); itr != instance_->browserMap_.end(); itr++)
    {
        delete itr->second;
    }

    instance_->browserMap_.clear();

    // shutdown subsystems
    UWKRenderer::Shutdown();
    UWKMessageQueue::Shutdown();

    if (instance_->renderMutex_)
        delete instance_->renderMutex_;

    if (instance_)
    {
        delete instance_;
        instance_ = NULL;
    }

}

void UWKServer::UpdateBrowserTextures()
{
    if (!TryLockRender())
        return;

    UWKRenderer::DeleteRenderers();

    for (std::map<uint32_t, UWKBrowser*>::iterator itr = browserMap_.begin(); itr != browserMap_.end(); itr++)
    {
        if (!itr->second)
        {
            continue;
        }

        itr->second->UpdateTexture();
    }

    UnlockRender();
}

bool UWKServer::TryLockRender()
{
    if (!instance_)
        return false;

    if (!instance_->renderMutex_)
        return true;

    return instance_->renderMutex_->tryLock();

}

void UWKServer::LockRender()
{
    if (!instance_)
        return;

    if (!instance_->renderMutex_)
        return;

    instance_->renderMutex_->lock();
}

void UWKServer::UnlockRender()
{
    if (!instance_)
        return;

    if (!instance_->renderMutex_)
        return;

    instance_->renderMutex_->unlock();
}

bool UWKServer::UsingRenderThread()
{
    if (!instance_ || !instance_->renderMutex_)
        return false;

    return true;
}

