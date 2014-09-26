/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/


#pragma once

#include <QObject>
#include <QMap>

class QWebFrame;

//#define UWK_USE_EXTERNAL_BRIDGE_JS

namespace UWK
{

class WebView;

// Usage, we can evaluate JS on the page with a string return

// JSObjects are on page, can send and receive JSON messages

// Global UWK object which can sendMessage and subscribe to a specific message

// properties JS -> C++ need to be known at compile time

class JSObject : public QObject
{
    Q_OBJECT

public:

};

// UWK object, provide sendMessage and subscribe
class JSBridge : public QObject
{

    Q_OBJECT

    static JSBridge* sInstance_;

    QMap<QString, JSObject*> properties_;

    // UWK.js
#ifdef UWK_USE_EXTERNAL_BRIDGE_JS
    QString jsUWK_;
#endif

signals:

    void messageReceived(const QString& msgName, const QString& msgJSON);

public:

    void AddBridgeToView(WebView *view);


    void ReceiveMessage(const QString& msgName, const QString& msgJSON);

    // returns true if object was newly created
    void RefreshViewProperties(WebView *view);
    void RefreshAllViewProperties();

    void RemoveObject(const QString& objectName);
    bool SetObjectProperty(const QString& objectName, const QString& propName, const QString& value);

    static JSBridge* Instance();
    static void Initialize();
    static void Shutdown();

    // send message from JS -> App
    Q_INVOKABLE void _sendMessage(unsigned int viewId, QString msgName, QString msgJSONValue);

};

}
