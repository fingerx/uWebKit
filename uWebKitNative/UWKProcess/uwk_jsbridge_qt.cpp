/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "UWKCommon/uwk_error.h"
#include "uwk_jsbridge_qt.h"
#include "uwk_engine.h"
#include "uwk_webview.h"
#include "uwk_javascript_embedded.h"

#include <QFile>
#include <QWebFrame>

namespace UWK
{

JSBridge* JSBridge::sInstance_ = NULL;

JSBridge* JSBridge::Instance()
{
    if (!sInstance_)
    {
        UWKError::FatalError("JSBridge is not initialized");
        return NULL;
    }

    return sInstance_;
}

void JSBridge::RefreshViewProperties(WebView* view)
{
    QWebFrame* frame = view->page()->mainFrame();
    QMapIterator<QString, JSObject*> i(properties_);

    while (i.hasNext())
    {
        i.next();
        frame->addToJavaScriptWindowObject(i.key(), i.value());
    }

}

void JSBridge::RefreshAllViewProperties()
{
    QVector<WebView*> viewList;
    Engine::Instance()->GetViewList(viewList);

    for (int i = 0; i < viewList.size(); i++)
    {
        RefreshViewProperties(viewList[i]);
    }

}

void JSBridge::RemoveObject(const QString& objectName)
{
    if (!properties_.contains(objectName))
        return;

    JSObject* jso = properties_[objectName];
    properties_.remove(objectName);
    delete jso;

    RefreshAllViewProperties();
}

bool JSBridge::SetObjectProperty(const QString& objectName, const QString& propName, const QString& value)
{
    JSObject* jso = NULL;
    bool newjso = false;

    if (!properties_.contains(objectName))
    {
        newjso = true;
        jso = new JSObject();
        jso->setObjectName(objectName);
        properties_[objectName] = jso;
    }
    else
        jso = properties_[objectName];

    jso->setProperty(propName.toLatin1(), value);

    if (newjso)
        RefreshAllViewProperties();

    return newjso;
}

void JSBridge::AddBridgeToView(WebView* view)
{
    QWebFrame* frame = view->page()->mainFrame();

    frame->addToJavaScriptWindowObject(QString::fromLatin1("UWK"), this);

#ifdef UWK_USE_EXTERNAL_BRIDGE_JS
    frame->evaluateJavaScript(jsUWK_);
#else
    frame->evaluateJavaScript(JavascriptEmbedded::GetBridgeJS());
#endif

    setProperty("viewId", QVariant(view->GetID()));

    RefreshViewProperties(view);
}

void JSBridge::_sendMessage(unsigned int viewId, QString msgName, QString msgJSONValue)
{
    UWKMessage msg;
    msg.type = UMSG_JAVASCRIPT_MESSAGE;
    msg.browserID = viewId;
    UWKMessageQueue::SetString(msg, 0, msgName.toUtf8().constData());
    UWKMessageQueue::SetString(msg, 1, msgJSONValue.toUtf8().constData());
    UWKMessageQueue::Write(msg);
}

void JSBridge::ReceiveMessage(const QString& msgName, const QString& msgJSON)
{

    emit messageReceived(msgName, msgJSON);

}

void JSBridge::Initialize()
{
    if (sInstance_)
    {
        UWKError::FatalError("JSBridge is already initialized");
        return;
    }

    sInstance_ = new JSBridge();

#ifdef UWK_USE_EXTERNAL_BRIDGE_JS

#ifndef _MSC_VER
    const char* uwkjsPath = "/Users/josh/Dev/ThunderBeast/uWebKit2/uWebKitNative/Javascript/UWK.js";
#else
    const char* uwkjsPath = "C:/Dev/ThunderBeast/uWebKit2/uWebKitNative/Javascript/UWK.js";
#endif

    QFile jsfile(QString::fromLatin1(uwkjsPath));

    if (!jsfile.open(QFile::ReadOnly | QFile::Text))
    {
        UWKError::FatalError("JSBridge couldn't open %s", uwkjsPath);
        return;
    }

    QTextStream jsStream(&jsfile);
    sInstance_->jsUWK_ = jsStream.readAll();

    jsfile.close();

#endif


}

void JSBridge::Shutdown()
{
    if (!sInstance_)
        return;

    delete sInstance_;
    sInstance_ = NULL;

}

}
