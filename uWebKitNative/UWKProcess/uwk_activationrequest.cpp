
#include <QApplication>
#include <QtGui>
#include <QCryptographicHash>
#include <QUrlQuery>

#include "uwk_activationrequest.h"

namespace UWK
{

ActivationRequest::ActivationRequest(QString type) : m_manager(this), m_type(type)
{
    m_reply = NULL;
}

void ActivationRequest::addPostData(QString name, QString value)
{
    m_query.addQueryItem(name, value);
}

void ActivationRequest::loadURL(QUrl url)
{
    m_request.setUrl(url);
    m_request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/x-www-form-urlencoded"));

    m_postData = m_query.query(QUrl::FullyEncoded).toUtf8();

    m_reply = m_manager.post(m_request, m_postData);
}

void ActivationRequest::replyFinished(QNetworkReply* reply)
{
    // Reading attributes of the reply
    // e.g. the HTTP status code
    QVariant statusCodeV =
            reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    // Or the target URL if it was a redirect:
    QVariant redirectionTargetUrl =
            reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    // see CS001432 on how to handle this

    // no error received?
    if (reply->error() == QNetworkReply::NoError)
    {
        // read data from QNetworkReply here

        // Example 1: Creating QImage from the reply
        //QImageReader imageReader(reply);
        //QImage pic = imageReader.read();

        // Example 2: Reading bytes form the reply
        //QByteArray bytes = reply.readAll();  // bytes
        //QString string(bytes); // string
    }
    // Some http error received
    else
    {
        // handle errors here
    }

    // We receive ownership of the reply object
    // and therefore need to handle deletion.
    delete reply;
}

}



