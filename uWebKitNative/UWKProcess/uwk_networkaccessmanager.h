/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkProxy>

namespace UWK {

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit NetworkAccessManager(QObject *parent = 0);
    virtual QNetworkReply* createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );

    QString m_authUsername;
    QString m_authPassword;

    QString m_proxyUsername;
    QString m_proxyPassword;

    void setAuthCredentials(const QString& username, const QString& password) {
        m_authUsername = username;
        m_authPassword = password;
        
    }

    void setProxyCredentials(const QString& username, const QString& password) {
        m_proxyUsername = username;
        m_proxyPassword = password;
    }

signals:

public slots:

    void provideAuthentication(QNetworkReply *reply, QAuthenticator *auth) {
        Q_UNUSED(reply);

        auth->setUser(m_authUsername);
        auth->setPassword(m_authPassword);
    }

    void proxyAuthenticationRequired ( const QNetworkProxy & proxy, QAuthenticator *auth ) {
        Q_UNUSED(proxy);
        auth->setUser(m_proxyUsername);
        auth->setPassword(m_proxyPassword);

    }

};

}

#endif // NETWORKACCESSMANAGER_H
