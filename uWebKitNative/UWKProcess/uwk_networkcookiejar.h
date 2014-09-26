/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QNetworkCookie>
#include <QNetworkCookieJar>

namespace UWK {

class NetworkCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit NetworkCookieJar(QObject *parent = 0);

    void save();
    void load();
    void clear();

signals:
    void cookiesChanged();

public slots:

private:
    bool m_loaded;

    void purgeOldCookies();

};

}

#endif // COOKIEJAR_H
