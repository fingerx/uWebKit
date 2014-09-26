/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

struct UWKMessage;
class UWKBrowser;

class UWKRenderer
{
protected:

    uint32_t maxWidth_;
    uint32_t maxHeight_;
    UWKBrowser* browser_;

    // On Direct3D-like devices this returns a pointer to the base texture type (IDirect3DBaseTexture9 on D3D9, ID3D11Resource on D3D11).
    // On OpenGL-like devices the texture "name" is returned; cast the pointer to integer type to get it.
    void *nativeTexturePtr_;

    static std::vector<UWKRenderer*> sRendererDeleteQueue_;

public:

    UWKRenderer(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr);

    virtual ~UWKRenderer();

    virtual void Initialize(const UWKMessage& gpuSurfaceInfo) = 0;
    virtual bool IsValid() = 0;
    virtual void UpdateTexture() = 0;

    static UWKRenderer* CreateRenderer(UWKBrowser* browser, uint32_t flags, void* nativeTexturePtr);

    uintptr_t ParseGPUSurface(const UWKMessage& gpuSurfaceInfo);

    // called from main thread to delete renderer (and release graphics resources via Finalize
    // on the render thread
    void QueueRenderDelete();

    static void DeleteRenderers();

    static void Initialize();
    static void Shutdown();

};
