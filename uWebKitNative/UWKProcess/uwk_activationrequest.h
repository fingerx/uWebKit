
#pragma once

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>


#include <QtCore>
#include <QString>

namespace UWK {

    class ActivationRequest : public QObject
    {

        Q_OBJECT

    public:

        ActivationRequest(QString type);
        //~ActivationRequest();

        QString type() {return m_type;}

        void addPostData(QString name, QString value);

        void loadURL(QUrl url);

        QNetworkAccessManager* manager() { return &m_manager; }
        QNetworkRequest* request() { return &m_request; }
        QNetworkReply* reply() { return m_reply; }

    public slots:
        void replyFinished(QNetworkReply* reply);

    private:

        QNetworkAccessManager m_manager;
        QNetworkRequest m_request;
        QNetworkReply* m_reply;
        QUrlQuery m_query;

        QByteArray m_postData;

        QString m_type;

    };

}
