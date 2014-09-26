/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <string>
#include <map>

#include <Poco/Mutex.h>

#include "uwk_browser.h"
#include "uwk_renderer.h"

typedef void (*UWKProcessMessage)(UWKMessage* message);

class UWKServer
{
    static uint32_t sBrowserIDCounter_;
    static UWKServer* instance_;

    std::map<uint32_t, UWKBrowser*> browserMap_;

    UWKProcessMessage processCB_;
    Poco::Mutex* renderMutex_;

    UWKServer() : processCB_(NULL), renderMutex_(NULL) {}

public:

    static bool TryLockRender();
    static void LockRender();
    static void UnlockRender();
    static bool UsingRenderThread();

    static bool Initialize(UWKProcessMessage processCB, bool useRenderThread = false);
    static UWKServer* Instance() { return instance_; }
    static void Shutdown();

    void PumpMessages();
    void UpdateBrowserTextures();

    void Restart();

    UWKBrowser* CreateBrowser(uint32_t width, uint32_t height, std::string url, void* nativeTexturePtr);
    UWKBrowser* GetBrowser(uint32_t id);
    void DestroyBrowser(uint32_t id);
};
