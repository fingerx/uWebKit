/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_engine.h"
#include "uwk_networkaccessmanager.h"
#include "uwk_webview.h"
#include "uwk_webpage.h"
#include "uwk_qt_utilities.h"

#include "UWKCommon/uwk_message.h"

namespace UWK
{

WebPage::WebPage(WebView* view) :
    QWebPage(view)
{
    view_ = view;

    setNetworkAccessManager(Engine::Instance()->GetNetworkAccessManager());

    //connect(this, SIGNAL(repaintRequested(const QRect& dirtyRect)),
    //        this, SLOT(repaintRequested(const QRect& dirtyRect)));

}

void WebPage::javaScriptAlert ( QWebFrame* frame, const QString& message)
{
    Q_UNUSED(frame);
    UWKMessage msg;
    msg.type = UMSG_JAVASCRIPT_CONSOLE;
    msg.browserID = view_->GetID();
    msg.iParams[0] = 0;
    QtUtils::SetMessageQString(msg, 0, message);
    QtUtils::SetMessageQString(msg, 1, QString::fromLatin1("Javascript Alert"));
    UWKMessageQueue::Write(msg);
}

bool WebPage::javaScriptConfirm (QWebFrame* frame, const QString& message )
{
    Q_UNUSED(frame);
    UWKMessage msg;
    msg.type = UMSG_JAVASCRIPT_CONSOLE;
    msg.browserID = view_->GetID();
    msg.iParams[0] = 0;
    QtUtils::SetMessageQString(msg, 0, message);
    QtUtils::SetMessageQString(msg, 1, QString::fromLatin1("Javascript Confirm"));
    UWKMessageQueue::Write(msg);
    return false;
}


bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{

    if (frame == NULL)
    {
        // new window requests load them in same frame, look into adding popups
        // mainFrame()->load(request.url());

        UWKMessage msg;
        msg.type = UMSG_VIEW_REQUESTNEWVIEW;
        msg.browserID = view_->GetID();
        QtUtils::SetMessageQString(msg, 0, request.url().toString());
        UWKMessageQueue::Write(msg);

        return false;

    }

    return QWebPage::acceptNavigationRequest(frame, request, type);
}


void WebPage::repaintRequested(const QRect& dirtyRect)
{
    QWebPage::repaintRequested(dirtyRect);
    //UWKLog::LogVerbose("repaintRequested: %i x %i", dirtyRect.width(), dirtyRect.height());
}

void WebPage::javaScriptConsoleMessage ( const QString &message, int lineNumber, const QString &sourceID )
{

    UWKMessage msg;
    msg.type = UMSG_JAVASCRIPT_CONSOLE;
    msg.browserID = view_->GetID();
    msg.iParams[0] = lineNumber;
    QtUtils::SetMessageQString(msg, 0, message);
    QtUtils::SetMessageQString(msg, 1, sourceID);
    UWKMessageQueue::Write(msg);

    QWebPage::javaScriptConsoleMessage(message, lineNumber, sourceID);

}

QString WebPage::userAgentForUrl(const QUrl & url) const
{
    if (userAgentOverride_.length())
        return userAgentOverride_;

    return QWebPage::userAgentForUrl(url);
}


}
