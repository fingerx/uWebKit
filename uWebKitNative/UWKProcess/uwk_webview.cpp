/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/


#include "UWKCommon/uwk_keyboard.h"
#include "UWKCommon/uwk_config.h"

#include "uwk_engine.h"
#include "uwk_webpage.h"
#include "uwk_webview.h"
#include "uwk_gpusurface.h"

#include "uwk_qt_utilities.h"

#include "uwk_networkcookiejar.h"
#include "uwk_jsbridge_qt.h"

#include "uwk_javascript_embedded.h"

namespace UWK
{

WebView::WebView(uint32_t id, int maxWidth, int maxHeight) :
    id_(id),
    maxWidth_(maxWidth),
    maxHeight_(maxHeight),
    currentWidth_(-1),
    currentHeight_(-1),
    gpuImage_(maxWidth_, maxHeight_, QImage::Format_ARGB32_Premultiplied),
    frameContentsSize_(-1, -1),
    rendering_(false),
    cursorDraw_(true),
    cursorBlinkTime_(QDateTime::currentDateTime()),
    visible_(true),
    progress_(0),
    alphaMaskEnabled_(false)
{
    page_ = new WebPage(this);
    setPage(page_);

    // take a snapshot of the original palette
    originalPagePalette_ = page_->palette();

    buttonState_[0] = false;
    buttonState_[1] = false;
    buttonState_[2] = false;

    gpuSurface_ = GpuSurface::Create(maxWidth, maxHeight);

    scene_ = new QGraphicsScene();
    graphicsView_ = new QGraphicsView(scene_);
    scene_->addItem(this);

    // this is only a factor if QWebSettings::TiledBackingStoreEnabled is enabled
    // though, after some testing it seems TiledBackingStore has numerous problems
    setTiledBackingStoreFrozen(false);

    graphicsView_->setInteractive(true);
    graphicsView_->setCacheMode(QGraphicsView::CacheNone);
    graphicsView_->setOptimizationFlag(QGraphicsView::DontSavePainterState, true);
    graphicsView_->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);

    adjustSize(maxWidth, maxHeight);

    // signals
    connect(page()->mainFrame(), SIGNAL(loadStarted()),
            this, SLOT(loadStarted()));

    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished(bool)));

    connect(this, SIGNAL(urlChanged(const QUrl&)),
            this, SLOT(urlChanged(const QUrl&)));

    connect(this, SIGNAL(titleChanged(const QString&)),
            this, SLOT(titleChanged(const QString&)));

    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(setProgress(int)));

    connect(page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(javaScriptWindowObjectCleared()));

    connect(page()->networkAccessManager(),
            SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>& )),
            this,
            SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError>& )));

    timerId_ = startTimer(33);

#ifdef GITHUB_BUILD
    githubText_.setText(QString::fromLatin1("uWebKit GitHub Build<br>contact sales@uwebkit.com<br>for a Source License"));
    githubText_.prepare(QTransform(), QFont(QString::fromLatin1("Arial"), 18));
#endif

}

WebView::~WebView()
{
    killTimer(timerId_);

    scene_->removeItem(this);

    delete gpuSurface_;
    delete page_;
    delete graphicsView_;
    delete scene_;
}

void WebView::adjustSize(int width, int height)
{
    if (width == currentWidth_ && height == currentHeight_)
        return;

    currentWidth_ = width;
    currentHeight_ = height;

    pageImage_ = QImage(width, height, QImage::Format_ARGB32_Premultiplied);

    scene_->setSceneRect(QRect(0, 0, width, height));

    graphicsView_->setFixedSize(width, height);
    graphicsView_->setFrameShape(QFrame::NoFrame);
    graphicsView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->resize(width, height);

    page()->setViewportSize(QSize(width, height));

}

void WebView::ProcessUWKMessage(const UWKMessage& msg)
{
    if (msg.type == UMSG_MOUSE_MOVE)
    {
        QPoint p = graphicsView_->mapFromScene(QPoint(msg.iParams[0], msg.iParams[1]));

        Qt::MouseButton button = Qt::NoButton;
        Qt::MouseButtons buttons = Qt::NoButton;

        if (buttonState_[ButtonLeft])
        {
            button = Qt::LeftButton;
            buttons = Qt::LeftButton;
        }

        QMouseEvent qm(QEvent::MouseMove, p, button, buttons,  Qt::NoModifier);
        QApplication::sendEvent(graphicsView_->viewport(), &qm);
    }
    else if (msg.type == UMSG_MOUSE_DOWN)
    {

        QFocusEvent focus(QEvent::FocusIn);
        QApplication::sendEvent(graphicsView_->viewport(), &focus);

        QPoint p = graphicsView_->mapFromScene(QPoint(msg.iParams[0], msg.iParams[1]));

        // necessary?
        grabMouse();

        Qt::MouseButton button = Qt::LeftButton;
        buttonState_[ButtonLeft] = true;
        QMouseEvent qm(QEvent::MouseButtonPress, p, button, button,  Qt::NoModifier);
        QApplication::sendEvent(graphicsView_->viewport(), &qm);

    }
    else if (msg.type == UMSG_MOUSE_UP)
    {
        QPoint p = graphicsView_->mapFromScene(QPoint(msg.iParams[0], msg.iParams[1]));

        // necessary?
        grabMouse();

        Qt::MouseButton button = Qt::LeftButton;
        buttonState_[ButtonLeft] = false;
        QMouseEvent qm(QEvent::MouseButtonRelease, p, button, button,  Qt::NoModifier);
        QApplication::sendEvent(graphicsView_->viewport(), &qm);
    }
    else if (msg.type == UMSG_MOUSE_SCROLL)
    {
        float scroll = msg.fParams[0];
        QWebFrame* frame = page()->currentFrame();
        if (frame)
        {
            int sbv = frame->scrollBarValue(Qt::Vertical);
            sbv -= int(scroll * 10.0f);
            frame->setScrollBarValue(Qt::Vertical, sbv);
        }

    }
    else if (msg.type == UMSG_KEY_DOWN || msg.type == UMSG_KEY_UP)
    {
        Qt::KeyboardModifiers keyMod = Qt::NoModifier;

        //msg.iParams[0] = keyCode;
        //msg.iParams[1] = modifiers;
        //msg.iParams[2] = keyEvent->Character;

        uint32_t mods = msg.iParams[1];

        if (mods & UWKKeyboard::Control)
            keyMod |= Qt::ControlModifier;

        if (mods & UWKKeyboard::Shift)
            keyMod |= Qt::ShiftModifier;

        if (mods & UWKKeyboard::Alt)
            keyMod |= Qt::AltModifier;

        if (mods & UWKKeyboard::CommandWin)
        {
#ifdef _MSC_VER
            keyMod |= Qt::MetaModifier;
#else
            keyMod |= Qt::ControlModifier;
#endif
        }

        if (msg.iParams[0] == Qt::Key_Tab || msg.iParams[2] == '\t')
        {
            if (msg.type == UMSG_KEY_UP)
            {
                if (mods & UWKKeyboard::Shift)
                    focusNextPrevChild(false);
                else
                    focusNextPrevChild(true);

                return;
            }
            else
            {
                return;
            }
        }

        QFocusEvent focus(QEvent::FocusIn);
        QApplication::sendEvent(graphicsView_->viewport(), &focus);

        grabKeyboard();

        char ascii[2];
        ascii[0] = (char) msg.iParams[2];
        ascii[1] = 0;

        if (mods)
            ascii[0] = 0;

        QKeyEvent ke(msg.type == UMSG_KEY_DOWN ? QEvent::KeyPress : QEvent::KeyRelease, msg.iParams[0], keyMod, QString::fromLatin1(ascii));

        QApplication::sendEvent(graphicsView_->viewport(), &ke);

        ungrabKeyboard();

        //UWKLog::LogVerbose("Key %s: %i %i %i %i", msg.type == UMSG_KEY_UP ? "Up" : "Down", (int) msg.type, msg.iParams[0], msg.iParams[1], msg.iParams[2]);
    }
    else if (msg.type == UMSG_VIEW_EVALUATE_JAVASCRIPT)
    {
        QString js = QtUtils::GetMessageQString(msg, 0);

        QVariant jresult = page()->mainFrame()->evaluateJavaScript(js);

        if (msg.asyncMessageId)
        {
            UWKMessage result;
            result.type = UMSG_ASYNC_RESULT;
            result.asyncMessageId = msg.asyncMessageId;
            QtUtils::SetMessageQString(result, 0, jresult.toString());

            UWKMessageQueue::Write(result);
        }

    }
    else if (msg.type == UMSG_JAVASCRIPT_MESSAGE)
    {
        QString msgName = QtUtils::GetMessageQString(msg, 0);
        QString jsonValue = QtUtils::GetMessageQString(msg, 1);
        JSBridge::Instance()->ReceiveMessage(msgName, jsonValue);

    }
    else if (msg.type == UMSG_VIEW_LOADURL)
    {
        QString url = QtUtils::GetMessageQString(msg, 0);
        load(url);
    }
    else if (msg.type == UMSG_VIEW_LOADHTML)
    {
        QString html = QtUtils::GetMessageQString(msg, 0);
        QString url = QtUtils::GetMessageQString(msg, 1);

        if (url.length())
        {
            QUrl baseURL(url);
            setHtml(html, baseURL);
        }
        else
        {
            setHtml(html);
        }
    }
    else if (msg.type == UMSG_VIEW_SHOW)
    {
        if (msg.iParams[0])
        {
            visible_ = true;
            show();
        }
        else
        {
            visible_ = false;
            hide();
        }
    }
    else if (msg.type == UMSG_VIEW_SETALPHAMASK)
    {
        if (msg.iParams[0])
        {
            setAlphaMask(true);
        }
        else
        {
            setAlphaMask(false);
        }
    }
    else if (msg.type == UMSG_VIEW_NAVIGATE)
    {
        if (msg.iParams[0] == 0)
        {
            page()->history()->forward();
        }
        else if (msg.iParams[0] == 1)
        {
            page()->history()->back();
        }
    }
    else if (msg.type == UMSG_VIEW_SETCURRENTSIZE)
    {
        adjustSize(msg.iParams[0], msg.iParams[1]);
    }
    else if (msg.type == UMSG_VIEW_SETZOOMFACTOR)
    {
        setZoomFactor(msg.fParams[0]);
    }
    else if (msg.type == UMSG_VIEW_SETSCROLLPOSITION)
    {
        page()->mainFrame()->setScrollPosition(QPoint(msg.iParams[0], msg.iParams[1]));
    }
    else if (msg.type == UMSG_IME_SETTEXT)
    {
        QString imeText =  QtUtils::GetMessageQString(msg, 0);
        setIME(imeText);
    }

}

uint32_t WebView::GetGPUSurfaceID()
{
    return gpuSurface_->GetSurfaceID();
}

void WebView::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    if (!visible_ || progress_ < 50)
        return;

    if (!rendering_)
    {
        rendering_ = true;

        if (page()->mainFrame()->contentsSize() != frameContentsSize_)
        {
            frameContentsSize_ = page()->mainFrame()->contentsSize();

            UWKMessage cmsg;
            cmsg.browserID = id_;
            cmsg.type = UMSG_VIEW_CONTENTSIZECHANGED;
            cmsg.iParams[0] = frameContentsSize_.width();
            cmsg.iParams[1] = frameContentsSize_.height();
            UWKMessageQueue::Write(cmsg);
        }

        gpuImage_.fill(Qt::transparent);
        pageImage_.fill(Qt::transparent);

        QPainter painter(&pageImage_);

        // if these are set to true and we're doing any scaling will
        // A) hide the fact at least partially as will look smooth
        // B) Be a huge performance hit
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setRenderHint(QPainter::TextAntialiasing, false);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

        scene_->render(&painter);

        QDateTime now = QDateTime::currentDateTime();

        if (cursorBlinkTime_.msecsTo(now) > 500)
        {
            cursorDraw_ = !cursorDraw_;
            cursorBlinkTime_ = now;
        }

        if (cursorDraw_ && inputMethodQuery(Qt::ImMaximumTextLength).toInt() > 0)
        {
            QRect r = inputMethodQuery(Qt::ImMicroFocus).toRect();
            painter.fillRect(r, 0xFF000000);

        }

        painter.end();

        QPainter gpuPainter(&gpuImage_);

        gpuPainter.drawImage(QPoint(0, 0), pageImage_);

#ifdef GITHUB_BUILD

        const QSizeF& size = githubText_.size();
        gpuPainter.fillRect(pageImage_.width() - size.width() - 32, 0,
                            size.width() + 16, size.height() + 8, QColor(32, 32, 32,192) );

        gpuPainter.setPen(Qt::white);
        gpuPainter.drawStaticText(pageImage_.width() - size.width() - 24, 0, githubText_);
#endif

        gpuPainter.end();

        gpuSurface_->UpdateTexture(gpuImage_.bits());

        rendering_ = false;


    }
}

void WebView::loadStarted()
{
    progress_ = 0;
}

void WebView::loadFinished(bool ok)
{
    Q_UNUSED(ok);

    UWKMessage msg;
    msg.browserID = id_;
    msg.type = UMSG_VIEW_LOADFINISHED;
    UWKMessageQueue::Write(msg);

    progress_ = 100;

    checkIcon();

    Engine::Instance()->GetCookieJar()->save();
}

void WebView::urlChanged(const QUrl& url)
{

    UWKMessage msg;
    msg.browserID = id_;
    msg.type = UMSG_VIEW_URLCHANGED;

    QString string = url.toString();

    UWKMessageQueue::SetString(msg, 0, string.toUtf8().constData());

    UWKMessageQueue::Write(msg);

    checkIcon();

}

void WebView::titleChanged(const QString& title)
{

    UWKMessage msg;
    msg.browserID = id_;
    msg.type = UMSG_VIEW_TITLECHANGED;

    UWKMessageQueue::SetString(msg, 0, title.toUtf8().constData());

    UWKMessageQueue::Write(msg);

    checkIcon();

}

void WebView::setProgress( int progress)
{
    progress_ = progress;
    UWKMessage msg;
    msg.browserID = id_;
    msg.type = UMSG_VIEW_LOADPROGRESS;
    msg.iParams[0] = progress;
    UWKMessageQueue::Write(msg);
}

void WebView::checkIcon()
{
    if (icon().isNull())
        return;

    QImage image(icon().pixmap(16,16).toImage());
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    image.save(&buffer, "PNG");

    if (cachedIcon_.data().count() == buffer.data().count())
    {
        if (!memcmp(cachedIcon_.data(), buffer.data(), cachedIcon_.data().count()))
            return;
    }

    cachedIcon_.setData(buffer.data(), buffer.data().count());

    UWKMessage msg;
    msg.browserID = id_;
    msg.type = UMSG_VIEW_ICONCHANGED;
    msg.iParams[0] = 16;
    msg.iParams[1] = 16;

    if (UWKMessageQueue::AllocateAndCopy(msg, 0, (const void *) buffer.data().constData(), buffer.data().count()))
    {
        UWKMessageQueue::Write(msg);
    }
}

void WebView::javaScriptWindowObjectCleared()
{
    registerIMEHandler();
    JSBridge::Instance()->AddBridgeToView(this);
}

void WebView::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
    reply->ignoreSslErrors();
}


void WebView::setIME(const QString& ime)
{
    page()->mainFrame()->documentElement().evaluateJavaScript(QString::fromLatin1("UWK_TextElement.value = \"") + ime + QString::fromLatin1("\";"));
}


void WebView::handleFocusIn(QVariant type, QVariant var, QVariant left, QVariant top, QVariant width, QVariant height)
{

    UWKMessage msg;
    msg.browserID = id_;
    msg.type = UMSG_IME_FOCUSIN;

    // rect
    msg.iParams[0] = left.toInt();
    msg.iParams[1] = top.toInt();
    msg.iParams[2] = width.toInt();
    msg.iParams[3] = height.toInt();

    // type
    UWKMessageQueue::SetString(msg, 0,  type.toString().toUtf8().constData());
    // value
    UWKMessageQueue::SetString(msg, 1,  var.toString().toUtf8().constData());

    UWKMessageQueue::Write(msg);

}
void WebView::handleFocusOut(QVariant var)
{
    Q_UNUSED(var);

    UWKMessage msg;

    msg.browserID = id_;
    msg.type = UMSG_IME_FOCUSOUT;

    UWKMessageQueue::Write(msg);

}

void WebView::registerIMEHandler()
{
    if (!UWKConfig::GetIMEEnabled())
        return;

    page()->mainFrame()->addToJavaScriptWindowObject(QString::fromLatin1("UWK_View"), this);

    page()->mainFrame()->evaluateJavaScript(JavascriptEmbedded::GetIMEHandler());

}

void WebView::setAlphaMask(bool enabled)
{
    if (alphaMaskEnabled_ == enabled)
        return;

    if (enabled)
    {
        QPalette palette = page_->palette();
        palette.setBrush(QPalette::Base, Qt::transparent);
        page_->setPalette(palette);
        setAttribute(Qt::WA_OpaquePaintEvent, false);
    }
    else
    {
        page_->setPalette(originalPagePalette_);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
    }

    alphaMaskEnabled_ = enabled;
}

}
