/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/
#include "ThirdParty/glew/glew.h"
#include <OpenGL/CGLCurrent.h>
#include <OpenGL/CGLIOSurface.h>
#include "uwk_renderer_gl.h"
#include "uwk_browser.h"

bool UWKRendererGL::glCore_ = false;
bool UWKRendererGL::glewInitialized_ = false;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


static const char* kGlesVProgTextGLCore = "#version 150\n" \
    "in highp vec3 pos;\n"\
    "in highp vec2 uv;\n"\
    "in lowp  vec4 color;\n"\
    "out highp vec2 vTexCoord;\n"\
    "out lowp vec4 vColor;\n"\
    "\n"\
    "void main()\n" \
    "{\n"	\
    "	gl_Position = vec4(pos,1);\n"	\
    "	vTexCoord = uv;\n"	\
    "	vColor = color;\n"	\
    "}\n" \
    ;

static const char* kGlesFShaderTextGLCore = "#version 150\n" \
    "uniform sampler2DRect sDiffmap;\n" \
    "out lowp vec4 fragColor;\n"\
    "in highp vec2 vTexCoord;\n"\
    "in lowp vec4 vColor;\n"\
    "\n"\
    "void main()\n" \
    "{\n"	\
     "  vec4 diffInput = texture(sDiffmap, vTexCoord);\n" \
     "	fragColor = diffInput * vColor;\n"	\
    "}\n" \
    ;


struct uWebkitVertex {
    float x, y, z;
    float u, v;
    unsigned int color;
};

enum
{
    ATTRIB_POSITION = 0,
    ATTRIB_UV = 1,
    ATTRIB_COLOR = 2
};

static float identityMatrix[16] = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1,
};

static uWebkitVertex verts[6] = {

    { -1, -1, 0, 0, 0, 0xFFffffff },
    {  1, 1,  0, 1, 0, 0xFFffffff },
    { -1, 1,  0, 0, 1, 0xFFffffff },

    {  1, 1,  0, 0, 0,  0xFFffffff },
    {  1, -1,  0, 1, 0, 0xFFffffff },
    { -1, 1,   0, 0, 1, 0xFFffffff }

};


UWKRendererGL::UWKRendererGL(uint32_t maxWidth, uint32_t maxHeight, void *nativeTexturePtr) :
    UWKRenderer(maxWidth, maxHeight, nativeTexturePtr),
    surfaceIDSet_(false), valid_(false)
{

}

void UWKRendererGL::renderToTextureGLCore()
{
    if (!valid_)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID_);

    glViewport(0, 0, maxWidth_, maxHeight_);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, surfaceTextureID_);
    glUniform1i(diffMapSamplerIndex_, 0);

    verts[0].x = -1;
    verts[0].y = 1;
    verts[1].x = 1;
    verts[1].y = 1;
    verts[2].x = -1;
    verts[2].y = -1;

    verts[0].u = 0;
    verts[0].v = 0;
    verts[1].u = maxWidth_;
    verts[1].v = 0;
    verts[2].u = 0;
    verts[2].v = maxHeight_;

    verts[3].x = -1;
    verts[3].y = -1;
    verts[4].x = 1;
    verts[4].y = 1;
    verts[5].x = 1;
    verts[5].y = -1;

    verts[3].u = 0;
    verts[3].v = maxHeight_;
    verts[4].u = maxWidth_;
    verts[4].v = 0;
    verts[5].u = maxWidth_;
    verts[5].v = maxHeight_;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uWebkitVertex) * 6, &verts[0].x);

    glEnableVertexAttribArray(ATTRIB_POSITION);
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(uWebkitVertex), BUFFER_OFFSET(0));

    glEnableVertexAttribArray(ATTRIB_UV);
    glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, sizeof(uWebkitVertex), BUFFER_OFFSET(sizeof(float) * 3));

    glEnableVertexAttribArray(ATTRIB_COLOR);
    glVertexAttribPointer(ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(uWebkitVertex), BUFFER_OFFSET(sizeof(float) * 5));

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_RECTANGLE, 0);

    // Render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void UWKRendererGL::renderToTexture()
{
    if (glCore_)
        renderToTextureGLCore();
    else
        renderToTextureGL2();
}

void UWKRendererGL::renderToTextureGL2()
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

void UWKRendererGL::InitInternalGLCore()
{

    glGetError();

    vertexShader_ = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader_, 1, &kGlesVProgTextGLCore, NULL);
    glCompileShader(vertexShader_);

    GLenum error = glGetError();

    fragmentShader_ = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader_, 1, &kGlesFShaderTextGLCore, NULL);
    glCompileShader(fragmentShader_);

    error = glGetError();

    glGenBuffers(1, &arrayBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, arrayBuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uWebkitVertex) * 6, NULL, GL_STREAM_DRAW);

    shaderProgram_ = glCreateProgram();
    glBindAttribLocation(shaderProgram_, ATTRIB_POSITION, "pos");
    glBindAttribLocation(shaderProgram_, ATTRIB_COLOR, "color");
    glBindAttribLocation(shaderProgram_, ATTRIB_UV, "uv");
    glAttachShader(shaderProgram_, vertexShader_);
    glAttachShader(shaderProgram_, fragmentShader_);

    glBindFragDataLocation(shaderProgram_, 0, "fragColor");

    error = glGetError();

    glLinkProgram(shaderProgram_);

    error = glGetError();

    GLint status = 0;
    glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &status);
    //assert(status == GL_TRUE);

    error = glGetError();

    glUseProgram(shaderProgram_);
    diffMapSamplerIndex_ =  glGetUniformLocation(shaderProgram_, "sDiffmap");
    glUseProgram(0);

}

void UWKRendererGL::InitInternal()
{
    if (!glewInitialized_)
    {
        glewExperimental = GL_TRUE;
        glewInit();
        glGetError(); // Clean up error generated by glewInit
        glewInitialized_ = true;
    }

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

    if (glCore_)
        InitInternalGLCore();

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

        if (glCore_)
        {
            glDeleteProgram(shaderProgram_);
            glDeleteShader(vertexShader_);
            glDeleteShader(fragmentShader_);
            glDeleteBuffers(1, &arrayBuffer_);

        }
    }

    //glCore_ = false;
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





