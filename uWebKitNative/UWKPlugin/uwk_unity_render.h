/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

// Which platform we are on?
#if _MSC_VER
#define UNITY_WIN 1
#else
#define UNITY_OSX 1
#endif


// Attribute to make function be exported from a plugin
#if UNITY_WIN
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif


// Which graphics device APIs we possibly support?
#if UNITY_WIN
#define SUPPORT_D3D9 1
#define SUPPORT_D3D11 1 // comment this out if you don't have D3D11 header/library files
//#define SUPPORT_OPENGL 1
#endif

#if UNITY_OSX
#define SUPPORT_OPENGL 1
#endif


// Graphics device identifiers in Unity

typedef enum GfxDeviceRenderer
{
    kUnityGfxRendererOpenGL            =  0, // Desktop OpenGL 2 (deprecated)
    kUnityGfxRendererD3D9              =  1, // Direct3D 9
    kUnityGfxRendererD3D11             =  2, // Direct3D 11
    kUnityGfxRendererGCM               =  3, // PlayStation 3
    kUnityGfxRendererNull              =  4, // "null" device (used in batch mode)
    kUnityGfxRendererXenon             =  6, // Xbox 360
    kUnityGfxRendererOpenGLES20        =  8, // OpenGL ES 2.0
    kUnityGfxRendererOpenGLES30        = 11, // OpenGL ES 3.x
    kUnityGfxRendererGXM               = 12, // PlayStation Vita
    kUnityGfxRendererPS4               = 13, // PlayStation 4
    kUnityGfxRendererXboxOne           = 14, // Xbox One
    kUnityGfxRendererMetal             = 16, // iOS Metal
    kUnityGfxRendererOpenGLCore        = 17, // Desktop OpenGL core
    kUnityGfxRendererD3D12             = 18, // Direct3D 12
} UnityGfxRenderer;


// Event types for UnitySetGraphicsDevice
enum GfxDeviceEventType {
    kGfxDeviceEventInitialize = 0,
    kGfxDeviceEventShutdown,
    kGfxDeviceEventBeforeReset,
    kGfxDeviceEventAfterReset,
};


// If exported by a plugin, this function will be called when graphics device is created, destroyed,
// before it's being reset (i.e. resolution changed), after it's being reset, etc.
//extern "C" void EXPORT_API UnitySetGraphicsDevice (void* device, int deviceType, int eventType);

// If exported by a plugin, this function will be called for GL.IssuePluginEvent script calls.
// The function will be called on a rendering thread; note that when multithreaded rendering is used,
// the rendering thread WILL BE DIFFERENT from the thread that all scripts & other game logic happens!
// You have to ensure any synchronization with other plugin script calls is properly done by you.
//extern "C" void EXPORT_API UnityRenderEvent (int eventID);

