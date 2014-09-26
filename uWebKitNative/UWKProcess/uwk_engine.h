
/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/


#include <QMap>
#include <QUrl>

#include "UWKCommon/uwk_common.h"
#include "UWKCommon/uwk_message.h"

namespace UWK
{

class NetworkAccessManager;
class NetworkCookieJar;

class WebView;

class Engine
{
    QMap<uint32_t, WebView*> viewMap_;

    bool shutdown_;

    static Engine* sInstance_;

    NetworkAccessManager* networkAccessManager_;

    void CreateWebView(uint32_t id, int maxWidth, int maxHeight, const QUrl& initialURL);

    Engine();

public:

    static void Initialize();
    static Engine* Instance() { return sInstance_; }
    static void Shutdown();

    void Update();
    void ProcessUWKMessage(const UWKMessage& msg);

    void GetViewList(QVector<WebView*>& viewList);

    bool ShutdownReceived() { return shutdown_; }

    NetworkAccessManager* GetNetworkAccessManager();
    NetworkCookieJar* GetCookieJar();

};

}
