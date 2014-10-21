/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <QtGui>
#include <QtWebKit>
#include <QStyleFactory>

#include "UWKCommon/uwk_config.h"
#include "UWKCommon/uwk_process_client.h"
#include "uwk_application.h"
#include "uwk_engine.h"
#include "uwk_javascript_embedded.h"
#include "uwk_activation.h"

namespace UWK
{


UWKApplication::UWKApplication(int &argc, char **argv)
    : QApplication(argc, argv), timerId_(0)
{

    std::string base64Config;

#ifdef _MSC_VER
    QString cmdline = QString::fromUtf16((unsigned short *)GetCommandLineW());
    QStringList args = cmdline.split(QString::fromLatin1(" "));

    // This loads everything but the qwindows platform plugin from the deployed application folder
    QString pluginPath = QCoreApplication::applicationDirPath();
    QStringList pluginPaths(pluginPath);
    QCoreApplication::setLibraryPaths(pluginPaths);

#else
    QStringList args = QCoreApplication::arguments();
#endif

    // -parentpid 1337 -processdb oe233wwwi

    QString _parentpid = QString::fromLatin1("-parentpid");
    QString _processdb = QString::fromLatin1("-processdb");

    uint32_t parentpid = 0;
    std::string processdb;
    for (int i = 0; i < args.count(); i++)
    {
        if (args.at(i) == _parentpid && i + 1 < args.count())
        {            
            parentpid = args.at(i + 1).toUInt();
            continue;
        }

        if (args.at(i) == _processdb && i + 1 < args.count())
        {
            processdb = QByteArray::fromBase64(args.at(i + 1).toUtf8()).constData();
            continue;
        }

    }

    if (!parentpid || !processdb.length())
        return;

    QNetworkProxyFactory::setUseSystemConfiguration(true);

    JavascriptEmbedded::Initialize();

    if (!UWKProcessClient::Initialize(parentpid, processdb))
        return;

    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);

    // plugins (Flash) disabled, will add an optional enable plugins, however default is off
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::DnsPrefetchEnabled, true);

    std::string _directory;
    UWKConfig::GetPersistentDataPath(_directory);
    QString directory = QString::fromLatin1(_directory.c_str());

    if (!QFile::exists(directory)) {
        QDir dir;
        dir.mkpath(directory);
    }

    QString iconsPath = directory + QString::fromLatin1("/icons");

    if (!QFile::exists(iconsPath)) {
        QDir dir;
        dir.mkpath(iconsPath);
    }

    QString localStoragePath = directory + QString::fromLatin1("/local");

    if (!QFile::exists(localStoragePath)) {
        QDir dir;
        dir.mkpath(localStoragePath);
    }

    // Only for QGraphicsView
    QWebSettings::globalSettings()->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true);

    // Unfortunately, as this helps performance a lot, this appears busted, it works fine if you don't scroll the page
    // QWebSettings::globalSettings()->setAttribute(QWebSettings::TiledBackingStoreEnabled, true);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    QWebSettings::globalSettings()->setOfflineStoragePath(localStoragePath);
    QWebSettings::globalSettings()->setOfflineWebApplicationCachePath(localStoragePath);
    QWebSettings::globalSettings()->setIconDatabasePath(iconsPath);

    // TODO: this is a Unity specific path
    std::string _cssPath;
    UWKConfig::GetDataPath(_cssPath);
    QString cssPath = QString::fromLatin1(_cssPath.c_str());
    cssPath += QString::fromLatin1("/StreamingAssets/uWebKit/Data/uwebkit.css");

    QUrl cssUrl(QString::fromUtf8("file://") + cssPath);
    QWebSettings::globalSettings()->setUserStyleSheetUrl(cssUrl);

    setStyle(QStyleFactory::create(QString::fromLatin1("Fusion")));

}

void UWKApplication::init()
{

    UWKLog::SetLogCallback(LogCallback);
    UWKError::SetErrorCallback(ErrorCallback);
    UWKMessageQueue::Initialize(false);

    Engine::Initialize();

    Activation::Initialize();

    //33hz
    timerId_ = startTimer(33);
}

void UWKApplication::update()
{
    PumpMessages();

    if (UWKProcessClient::Instance())
    {
        if (!UWKProcessClient::Instance()->Update())
        {
            shutdown();
            return;
        }
    }

    Engine::Instance()->Update();
}

void UWKApplication::timerEvent(QTimerEvent *event)
{
    event->accept();
    update();
}


void UWKApplication::shutdown()
{
    if (timerId_)
        killTimer(timerId_);

    if (UWKProcessClient::Instance())
        UWKProcessClient::Instance()->Shutdown();

    Engine::Shutdown();

    QCoreApplication::exit(0);
}

void UWKApplication::PumpMessages()
{

    UWKMessage msg;
    while (UWKMessageQueue::Read(msg))
    {

        Engine::Instance()->ProcessUWKMessage(msg);
        UWKMessageQueue::Dispose(msg);

        //UWKLog::LogVerbose("Message: %i %i %i %i", (int) msg.type, msg.iParams[0], msg.iParams[1], msg.iParams[2]);
    }

    if (Engine::Instance()->ShutdownReceived())
        shutdown();

}


void UWKApplication::LogCallback(const char* message, int level)
{
    if (!UWKMessageQueue::IsInitialized())
        return;

    UWKMessage msg;
    msg.type = UMSG_LOG;
    msg.iParams[0] = level;

    std::string str = "UWKProcess: ";
    str += + message;

    UWKMessageQueue::SetString(msg, 0, str.c_str());

    UWKMessageQueue::Write(msg);
}

void UWKApplication::ErrorCallback(const char* message, bool fatal)
{
    if (!UWKMessageQueue::IsInitialized())
        return;

    UWKMessage msg;
    msg.type = UMSG_ERROR;
    msg.iParams[0] = fatal ? 1 : 0;

    std::string str = "UWKProcess Error: ";
    str += + message;

    UWKMessageQueue::SetString(msg, 0, str.c_str());

    UWKMessageQueue::Write(msg);

}


}


