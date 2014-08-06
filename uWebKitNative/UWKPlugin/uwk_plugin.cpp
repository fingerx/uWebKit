/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "UWKCommon/uwk_log.h"
#include "UWKCommon/uwk_error.h"
#include "UWKCommon/uwk_config.h"
#include "UWKCommon/uwk_message.h"
#include "UWKCommon/uwk_server.h"
#include "UWKCommon/uwk_process_server.h"

#include "uwk_unity_keys.h"
#include "uwk_unity_beta.h"
#include "uwk_plugin.h"


#ifdef _MSC_VER
    #include <windows.h>
    #define EXPORT_API __declspec(dllexport)
#else
    #define EXPORT_API
#endif


void UWKPlugin::Initialize(UWKProcessMessage processCB, const char* utf8JsonConfig)
{

    sShutDown_ = false;

#ifdef UWK_UNITY_BETA

    // display current time stamp in log to setup beta
    //UWKUnityBeta::GetCurrentTimestamp();

    sBetaDaysLeft_ = UWKUnityBeta::GetBetaDaysLeft();

#endif

    std::string json(utf8JsonConfig);

    if (!UWKConfig::InitFromUnityJSON(json))
    {
        //TODO: Handle this
    }

    UWKServer::Initialize(processCB, true);

}

void UWKPlugin::Shutdown()
{
    UWKPlugin::sShutDown_ = true;

    UWKMessage msg;
    msg.type = UMSG_SHUTDOWN_WEBRENDERPROCESS;
    UWKMessageQueue::Write(msg);
    UWKServer::Shutdown();
}



int UWKPlugin::sBetaDaysLeft_ = 0;
bool UWKPlugin::sShutDown_ = false;
bool UWKPlugin::sHasDisplayedBetaMessage_ = false;

extern "C"
{

int EXPORT_API UWK_Initialize(UWKLogCallback logcb, UWKErrorCallback errorcb, UWKProcessMessage processcb, const char* utf8JsonConfig)
{
    UWKLog::SetLogCallback(logcb);
    UWKError::SetErrorCallback(errorcb);
    UWKPlugin::Initialize(processcb, utf8JsonConfig);

#ifdef UWK_UNITY_BETA
    return UWKPlugin::sBetaDaysLeft_ + 100;
#else

    return UWKPlugin::sBetaDaysLeft_;

#endif

}

void EXPORT_API UWK_Update()
{    
    UWKServer::Instance()->PumpMessages();
}

int EXPORT_API UWK_Shutdown()
{
    UWKPlugin::Shutdown();
    return 0;
}

bool EXPORT_API UWK_HasDisplayedBetaMessage()
{
    bool value = UWKPlugin::sHasDisplayedBetaMessage_;
    UWKPlugin::sHasDisplayedBetaMessage_ = true;
    return value;
}

uint32_t EXPORT_API UWK_CreateView(int maxWidth, int maxHeight, const char* url, void* nativeTexturePtr)
{
        std::string _url(url ? url : "");

#ifdef UWK_UNITY_BETA
    if (UWKPlugin::sBetaDaysLeft_ < 0)
        _url = "http://www.uwebkit.com";
#endif

    UWKBrowser* browser = UWKServer::Instance()->CreateBrowser(maxWidth, maxHeight, _url, nativeTexturePtr);

    if (!browser)
        return UWK_INVALID_BROWSER_ID;
    else
        return browser->GetID();
}

EXPORT_API void* UWK_GetMessageDataPtr(UWKMessage* message, int index)
{
    void* data;
    uint32_t dataSize;

    if (!UWKMessageQueue::GetData(*message, index, &data, dataSize) || !data || !dataSize)
        return NULL;

    return data;
}

EXPORT_API bool UWK_GetMsgDataBytes (UWKMessage* message, int index, void* pout, int size)
{
    void* data;
    uint32_t dataSize;

    if (!UWKMessageQueue::GetData(*message, index, &data, dataSize) || !data || !dataSize)
        return false;

    if ((int)dataSize != size)
        return false;

    memcpy(pout, data, size);

    return true;

}

EXPORT_API int UWK_GetMsgDataSize (UWKMessage* message, int index)
{
    void* data;
    uint32_t dataSize;

    if (!UWKMessageQueue::GetData(*message, index, &data, dataSize) || !data || !dataSize)
        return 0;

    return (int) dataSize;

}

void EXPORT_API UWK_PostUnityKeyEvent(uint32_t id, UnityKeyEvent* keyEvent)
{
    /*
    if (keyEvent->Type == 1)
        UWKLog::LogVerbose("Key Down: %i", keyEvent->KeyCode);
    else
        UWKLog::LogVerbose("Key Up: %i", keyEvent->KeyCode);
        */

    UWKMessage msg;
    msg.browserID = id;
    if (EncodeUnityKeyEvent(keyEvent, msg))
        UWKMessageQueue::Write(msg);

}

void EXPORT_API UWK_DestroyView(uint32_t id)
{
    // We can get view destroys after core has shutdown
    if (UWKPlugin::sShutDown_)
        return;

    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_VIEW_DESTROY;
    UWKMessageQueue::Write(msg);

    UWKServer::Instance()->DestroyBrowser(id);

}


void EXPORT_API UWK_ClearCookies()
{
    UWKMessage msg;
    msg.browserID = -1;
    msg.type = UMSG_COOKIES_CLEAR;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgMouseMove(uint32_t id, int x, int y)
{
    //UWKLog::LogVerbose("Mouse Move: %i %i", x, y);

    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_MOUSE_MOVE;
    msg.iParams[0] = x;
    msg.iParams[1] = y;
    UWKMessageQueue::Write(msg);

}

void EXPORT_API UWK_MsgMouseButtonDown(uint32_t id, int x, int y, int button)
{

    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_MOUSE_DOWN;
    msg.iParams[0] = x;
    msg.iParams[1] = y;
    msg.iParams[2] = button;
    UWKMessageQueue::Write(msg);

    //UWKLog::LogVerbose("Mouse Down: %i %i %i", x, y, button);
}


void EXPORT_API UWK_MsgMouseButtonUp(uint32_t id, int x, int y, int button)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_MOUSE_UP;
    msg.iParams[0] = x;
    msg.iParams[1] = y;
    msg.iParams[2] = button;
    UWKMessageQueue::Write(msg);

    //UWKLog::LogVerbose("Mouse Up: %i %i %i", x, y, button);
}

void EXPORT_API UWK_MsgMouseScroll(uint32_t id, int x, int y, float scroll)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_MOUSE_SCROLL;
    msg.iParams[0] = x;
    msg.iParams[1] = y;
    msg.fParams[0] = scroll;
    UWKMessageQueue::Write(msg);

    //UWKLog::LogVerbose("Mouse Up: %i %i %i", x, y, button);
}


// first example of an async command, we want the return value
// separate read and write queues
uint32_t EXPORT_API UWK_MsgEvaluateJavascript(uint32_t id, const char* utf8Javascript, UWKAsyncMessageCallback callback)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_VIEW_EVALUATE_JAVASCRIPT;
    UWKMessageQueue::SetString(msg, 0, utf8Javascript);

    // if callback write an async message
    if (callback)
    {
        UWKMessageQueue::WriteAsync(msg, callback);
    }
    else
    {
        UWKMessageQueue::Write(msg);
    }

    return msg.asyncMessageId;;

    //UWKLog::LogVerbose("Mouse Up: %i %i %i", x, y, button);
}

void EXPORT_API UWK_MsgLoadHTML(uint32_t id, const char* utf8HTML, const char* utf8BaseURL)
{
#ifdef UWK_UNITY_BETA
    if (UWKPlugin::sBetaDaysLeft_ < 0)
        return;
#endif

    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_VIEW_LOADHTML;
    UWKMessageQueue::SetString(msg, 0, utf8HTML);
    UWKMessageQueue::SetString(msg, 1, utf8BaseURL);
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgJSMessage(uint32_t id, const char* utf8MsgName, const char* utf8JsonValue)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_JAVASCRIPT_MESSAGE;
    UWKMessageQueue::SetString(msg, 0, utf8MsgName);
    UWKMessageQueue::SetString(msg, 1, utf8JsonValue);
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_SetIMEText(uint32_t id, const char* utf8IMEText)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_IME_SETTEXT;
    UWKMessageQueue::SetString(msg, 0, utf8IMEText);
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgJSObjectSetProperty(const char* utf8ObjectName, const char* utf8PropertyName, const char* utf8Value)
{
    UWKMessage msg;
    msg.browserID = -1;
    msg.type = UMSG_JSOBJECT_SETPROPERTY;
    UWKMessageQueue::SetString(msg, 0, utf8ObjectName);
    UWKMessageQueue::SetString(msg, 1, utf8PropertyName);
    UWKMessageQueue::SetString(msg, 2, utf8Value);
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgJSObjectRemove(const char* utf8ObjectName)
{
    UWKMessage msg;
    msg.browserID = -1;
    msg.type = UMSG_JSOBJECT_REMOVE;
    UWKMessageQueue::SetString(msg, 0, utf8ObjectName);
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgLoadURL(uint32_t id, const char* utf8URL)
{
    std::string url = utf8URL ? utf8URL : "";

#ifdef UWK_UNITY_BETA
    if (UWKPlugin::sBetaDaysLeft_ < 0)
        url = "http://www.uwebkit.com";
#endif


    UWKMessage msg;
    msg.type = UMSG_VIEW_LOADURL;
    msg.browserID = id;
    UWKMessageQueue::SetString(msg, 0, url.c_str());
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgSetAlphaMask(uint32_t id, bool enabled)
{
    UWKMessage msg;
    msg.type = UMSG_VIEW_SETALPHAMASK;
    msg.browserID = id;
    msg.iParams[0] = enabled ? 1 : 0;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgShow(uint32_t id, bool show)
{
    UWKBrowser* browser = UWKServer::Instance()->GetBrowser(id);

    if (browser)
        browser->SetVisible(show ? true : false);

    UWKMessage msg;
    msg.type = UMSG_VIEW_SHOW;
    msg.browserID = id;
    msg.iParams[0] = show ? 1 : 0;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgNavigate(uint32_t id, int value)
{
    UWKMessage msg;
    msg.type = UMSG_VIEW_NAVIGATE;
    msg.browserID = id;
    msg.iParams[0] = value;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgSetCurrentSize(uint32_t id, int width, int height)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_VIEW_SETCURRENTSIZE;
    msg.iParams[0] = width;
    msg.iParams[1] = height;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgSetScrollPosition(uint32_t id, int x, int y)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_VIEW_SETSCROLLPOSITION;
    msg.iParams[0] = x;
    msg.iParams[1] = y;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_MsgSetZoomFactor(uint32_t id, float zoom)
{
    UWKMessage msg;
    msg.browserID = id;
    msg.type = UMSG_VIEW_SETZOOMFACTOR;
    msg.fParams[0] = zoom;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_DevelopmentOnlyCrashProcess()
{
    abort();
}

void EXPORT_API UWK_DevelopmentOnlyCrashWebProcess()
{
    UWKMessage msg;
    msg.type = UMSG_DEV_CRASHWEBPROCESS;
    UWKMessageQueue::Write(msg);
}

void EXPORT_API UWK_DevelopmentOnlyHangWebProcess()
{
    UWKMessage msg;
    msg.type = UMSG_DEV_HANGWEBPROCESS;
    UWKMessageQueue::Write(msg);
}


}
