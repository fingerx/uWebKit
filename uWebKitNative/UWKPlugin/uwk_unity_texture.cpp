/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "UWKCommon/uwk_server.h"
#include "uwk_plugin.h"
#include "uwk_unity_render.h"


#ifdef _MSC_VER
#include <windows.h>
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

extern "C"
{
void EXPORT_API UnityRenderEvent (int eventID)
{
    if (eventID == 1)
    {
        UWKServer::Instance()->UpdateBrowserTextures();
    }
    else if (eventID == 2)
    {
        UWKServer::LockRender();

        UWKRenderer::DeleteRenderers();

        UWKServer::UnlockRender();
    }

}

}
