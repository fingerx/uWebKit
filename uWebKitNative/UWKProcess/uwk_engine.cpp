/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_engine.h"
#include "uwk_webview.h"

#include "uwk_networkcookiejar.h"
#include "uwk_networkaccessmanager.h"
#include "uwk_qt_utilities.h"
#include "uwk_jsbridge_qt.h"
#include "uwk_activation.h"

namespace UWK
{

Engine* Engine::sInstance_ = NULL;

Engine::Engine() : shutdown_(false), networkAccessManager_(NULL)
{

}

void Engine::Initialize()
{
    sInstance_ = new Engine();

    JSBridge::Initialize();

    // load the cookies, we should move this once the cookies are in SQLite db
    sInstance_->GetCookieJar()->load();

}

void Engine::Shutdown()
{
    if (!sInstance_)
        return;

    JSBridge::Shutdown();

    delete sInstance_;
    sInstance_ = NULL;
}



NetworkAccessManager* Engine::GetNetworkAccessManager()
{
    if (!networkAccessManager_)
    {
        networkAccessManager_ = new NetworkAccessManager();
        networkAccessManager_->setCookieJar(new NetworkCookieJar);
    }
    return networkAccessManager_;
}

NetworkCookieJar* Engine::GetCookieJar()
{
    return (NetworkCookieJar*) GetNetworkAccessManager()->cookieJar();
}


void Engine::CreateWebView(uint32_t id, int maxWidth, int maxHeight, const QUrl& initialURL)
{

    if (viewMap_.find(id) != viewMap_.end())
    {
        UWKError::FatalError("Engine::CreateWebView viewMap_ already contains WebView with id %u", id);
        return;
    }

    WebView* view = new WebView(id, maxWidth, maxHeight);
    viewMap_.insert(id, view);

    // only load if initial URL has length
    if (initialURL.toString().length() > 0)
        view->load(initialURL);

    // GPUSurfaceInfo
    UWKMessage msg;
    msg.type = UMSG_GPUSURFACE_INFO;
    msg.browserID = id;
    // flags
    msg.iParams[0] = (int32_t) view->GetGPUSurfaceFlags();

    uintptr_t surfaceID = view->GetGPUSurfaceID();
    UWKMessageQueue::AllocateAndCopy(msg, 0, (const void *) &surfaceID ,sizeof(uintptr_t));
    UWKMessageQueue::Write(msg);
}

void Engine::GetViewList(QVector<WebView*>& viewList)
{
    viewList.clear();
    for (QMap<uint32_t, WebView*>::Iterator itr = viewMap_.begin(); itr != viewMap_.end(); itr++)
    {
        viewList.push_back(itr.value());
    }
}

void Engine::Update()
{

}

void Engine::ProcessUWKMessage(const UWKMessage& msg)
{
    if (msg.type == UMSG_VIEW_CREATE)
    {
        std::string surl;
        UWKMessageQueue::GetString(msg, 0, surl);
        QUrl url(QString::fromUtf8(surl.c_str()));
        CreateWebView(msg.browserID, msg.iParams[0], msg.iParams[1], url);
        return;
    }

    if (msg.type == UMSG_VIEW_DESTROY)
    {
        if (!viewMap_.contains(msg.browserID))
        {
            // error
            return;
        }

        WebView* view = viewMap_.find(msg.browserID).value();
        viewMap_.remove(msg.browserID);
        delete view;


        return;
    }

    if (msg.type == UMSG_COOKIES_CLEAR)
    {
        GetCookieJar()->clear();
        return;
    }


    if (msg.type == UMSG_JSOBJECT_SETPROPERTY)
    {
        QString objectName = QtUtils::GetMessageQString(msg, 0);
        QString propertyName = QtUtils::GetMessageQString(msg, 1);
        QString value = QtUtils::GetMessageQString(msg, 2);

        JSBridge::Instance()->SetObjectProperty(objectName, propertyName, value);
        return;
    }

    if (msg.type == UMSG_JSOBJECT_REMOVE)
    {
        QString objectName = QtUtils::GetMessageQString(msg, 0);
        JSBridge::Instance()->RemoveObject(objectName);
        return;
    }

    if (msg.type == UMSG_SHUTDOWN_WEBRENDERPROCESS)
    {
        shutdown_ = true;
        return;
    }

    if (msg.type ==  UMSG_DEV_CRASHWEBPROCESS)
    {
        abort();
    }

    if (msg.type ==  UMSG_DEV_HANGWEBPROCESS)
    {
        for (int i = 0; i < 1; )
        {

        }
    }

    if (msg.type ==  UMSG_ACTIVATE)
    {
        Activation::Activate(QtUtils::GetMessageQString(msg, 0));
        return;
    }

    switch(msg.type)
    {
        case UMSG_MOUSE_MOVE:
        case UMSG_MOUSE_DOWN:
        case UMSG_MOUSE_UP:
        case UMSG_MOUSE_SCROLL:
        case UMSG_KEY_DOWN:
        case UMSG_KEY_UP:
        case UMSG_VIEW_EVALUATE_JAVASCRIPT:
        case UMSG_JAVASCRIPT_MESSAGE:
        case UMSG_VIEW_LOADURL:
        case UMSG_VIEW_LOADHTML:
        case UMSG_VIEW_SHOW:
        case UMSG_VIEW_SETALPHAMASK:
        case UMSG_VIEW_SETTEXTCARETCOLOR:
        case UMSG_VIEW_NAVIGATE:
        case UMSG_VIEW_RELOAD:
        case UMSG_VIEW_SETCURRENTSIZE:
        case UMSG_VIEW_SETZOOMFACTOR:
        case UMSG_VIEW_SETSCROLLPOSITION:
        case UMSG_IME_SETTEXT:
        case UMSG_VIEW_SETFRAMERATE:
        case UMSG_VIEW_SETUSERAGENT:
        case UMSG_VIEW_SHOWINSPECTOR:
        case UMSG_VIEW_CLOSEINSPECTOR:
        case UMSG_VIEW_STOP:
            if (!viewMap_.contains(msg.browserID))
                return;
            viewMap_.find(msg.browserID).value()->ProcessUWKMessage(msg);
            break;
    }

}

}
