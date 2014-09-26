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
#include <stdint.h>
#include <stdlib.h>

#define UWK_INVALID_BROWSER_ID 0xffffffff
#define UWK_MAX_BROWSERS 16

class UWKRenderer;

class UWKBrowser
{
    std::string currentURL_;
    uint32_t maxWidth_;
    uint32_t maxHeight_;
    uint32_t id_;
    uint32_t flags_;
    bool visible_;

    UWKRenderer* renderer_;
    void* nativeTexturePtr_;

public:

    UWKBrowser(uint32_t id, uint32_t maxWidth, uint32_t maxHeight);
    ~UWKBrowser();

    uint32_t GetID() { return id_; }
    uint32_t GetMaxWidth() { return maxWidth_; }
    uint32_t GetMaxHeight() { return maxHeight_; }

    void GetCurrentURL(std::string& url) { url = currentURL_; }
    void SetCurrentURL(const std::string& url) { currentURL_ = url; }

    void Restart();

    void CreateRenderer(uint32_t flags);
    void SetNativeTexture(void* nativeTexturePtr);

    void SetRenderer(UWKRenderer* renderer) { renderer_ = renderer; }
    UWKRenderer* GetRenderer() { return renderer_; }
    void DestroyRenderer();

    bool GetVisible() { return visible_; }
    void SetVisible(bool visible) { visible_ = visible; }

    void UpdateTexture();

};
