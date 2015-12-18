/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#pragma once

#include <IOSurface/IOSurface.h>
#include <OpenGL/gl.h>

#include "uwk_message.h"
#include "uwk_renderer.h"

class UWKSurfaceGL;

class UWKRendererGL : public UWKRenderer
{
    // IOSurfaceID is defined as uint32_t defined
    IOSurfaceID surfaceID_;
    IOSurfaceRef surfaceRef_;
    bool surfaceIDSet_;

    // texture backed by IOSurface
    GLuint surfaceTextureID_;

    GLuint framebufferID_;

    // texture rendered by Unity in user scene
    GLuint targetTextureID_;

    bool valid_;

    static bool glCore_;

    void renderToTextureGL2();
    void renderToTextureGLCore();
    void renderToTexture();

    bool setupFrameBuffer();

    // called from GL thread in Unity
    void InitInternalGLCore();
    void InitInternal();

    // GLCore
    GLuint	vertexShader_;
    GLuint	fragmentShader_;
    GLuint	shaderProgram_;
    GLuint	arrayBuffer_;

    int diffMapSamplerIndex_;

public:

    static void SetGLCore(bool value) { glCore_ = value; }

    UWKRendererGL(uint32_t maxWidth, uint32_t maxHeight, void* nativeTexturePtr);
    virtual ~UWKRendererGL();

    void Initialize(const UWKMessage& gpuSurfaceInfo);
    bool IsValid() { return valid_; }
    void UpdateTexture();

};
