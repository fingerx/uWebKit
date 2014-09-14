/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <OpenGL/OpenGL.h>
#include "uwk_renderer_gl.h"
#include "uwk_browser.h"


UWKRendererGL::UWKRendererGL(uint32_t maxWidth, uint32_t maxHeight, void *nativeTexturePtr) :
    UWKRenderer(maxWidth, maxHeight, nativeTexturePtr),
    surfaceIDSet_(false), valid_(false)
{

}

void UWKRendererGL::renderToTexture()
{
    if (!valid_)
        return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glActiveTexture(GL_TEXTURE0);

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID_);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_MULTISAMPLE);
    glUseProgram(0);

    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, surfaceTextureID_);


    glViewport(0, 0, maxWidth_, maxHeight_);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, maxWidth_, maxHeight_, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();

    glBegin(GL_QUADS);

    glTexCoord2i(0,0);
    glVertex2f( 0, 0);

    glTexCoord2i(maxWidth_,0);
    glVertex2f( maxWidth_, 0);

    glTexCoord2i(maxWidth_,maxHeight_);
    glVertex2f( maxWidth_, maxHeight_);

    glTexCoord2i(0, maxHeight_);
    glVertex2f( 0, maxHeight_);

    glEnd();


    // pop texture matrix
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();


    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    glDisable(GL_TEXTURE_RECTANGLE_ARB);

    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glPopClientAttrib();
    glPopAttrib();
}

bool UWKRendererGL::setupFrameBuffer()
{
    glGenFramebuffers(1, &framebufferID_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID_);

    // the texture we're going to render to
    // the pointer should actually be a uint
    targetTextureID_ = (GLuint) size_t(nativeTexturePtr_);
    glBindTexture(GL_TEXTURE_2D, targetTextureID_);

    // Update with an empty image
    GLvoid* texClear = (GLvoid*) malloc(maxWidth_ * maxHeight_ * 4);
    memset(texClear, 0, maxWidth_ * maxHeight_ * 4);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, maxWidth_, maxHeight_, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
    free(texClear);

    // nearest filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // set "renderedTexture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTextureID_, 0);

    // set the list of draw buffers.
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers); // "1" is the size of DrawBuffers

    // always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }

    glViewport(0, 0, maxWidth_, maxHeight_);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void UWKRendererGL::InitInternal()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glActiveTexture(GL_TEXTURE0);

    CGLContextObj cgl_ctx = CGLGetCurrentContext();

    if (!setupFrameBuffer())
    {
        glPopClientAttrib();
        glPopAttrib();
        return;
    }

    glGenTextures(1, &surfaceTextureID_);
    glEnable(GL_TEXTURE_RECTANGLE_ARB);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, surfaceTextureID_);

    glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    CGLTexImageIOSurface2D(cgl_ctx, GL_TEXTURE_RECTANGLE_ARB, GL_RGBA, maxWidth_, maxHeight_, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,surfaceRef_, 0);

    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
    glDisable(GL_TEXTURE_RECTANGLE_ARB);

    glPopClientAttrib();
    glPopAttrib();

    // mark as valid
    valid_ = true;
}

void UWKRendererGL::Initialize(const UWKMessage &gpuSurfaceInfo)
{
    surfaceID_ = (IOSurfaceID) ParseGPUSurface(gpuSurfaceInfo);
    surfaceRef_ = IOSurfaceLookup(surfaceID_);

    //TODO:: make sure that surfaceID_ && IOSurfaceGetID(surfaceRef_) are equal once error system is in
    surfaceIDSet_ = true;

}

UWKRendererGL::~UWKRendererGL()
{
    if (valid_)
    {
        // this must be called on the main thread
        glDeleteFramebuffers(1, &framebufferID_);
        glDeleteTextures(1, &surfaceTextureID_);
    }

    valid_ = false;

}


void UWKRendererGL::UpdateTexture()
{
    if (!valid_ && surfaceIDSet_)
    {
        InitInternal();
    }

    renderToTexture();
}





