/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include <QtCore>
#include <QtGui/QDesktopServices>
#include <QtNetwork/QNetworkDiskCache>
#include <QtNetwork/QNetworkReply>

#include "UWKCommon/uwk_config.h"
#include "uwk_networkaccessmanager.h"

namespace UWK {

NetworkAccessManager::NetworkAccessManager(QObject *parent) : QNetworkAccessManager(parent)
{

    connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(provideAuthentication(QNetworkReply*,QAuthenticator*)));
    connect(this, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)));
    
    std::string persistentDataPath;
    UWKConfig::GetPersistentDataPath(persistentDataPath);
    QString location = QString::fromLatin1((persistentDataPath + "/cache").c_str());
    QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory(location);
    setCache(diskCache);
}


QNetworkReply* NetworkAccessManager::createRequest(Operation op, const QNetworkRequest & req, QIODevice * outgoingData)
{
    // copy
    QNetworkRequest request = req;

    //request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/28.0.1500.72 Safari/537.36");

    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    QNetworkReply* reply = QNetworkAccessManager::createRequest(op, request, outgoingData);
    return reply;
}



}



