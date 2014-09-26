
#include "UWKCommon/uwk_config.h"
#include "UWKCommon/uwk_process_client.h"
#include "uwk_activation.h"

namespace UWK
{

ActivationState Activation::activationState_ = ACTIVATION_REQUIRED;
int Activation::activationVersion_ = 1;
QString Activation::activationKey_;
ActivationRequest* Activation::request_ = NULL;
Activation* Activation::instance_ = NULL;


void Activation::Initialize()
{
    instance_ = new Activation();

    if (!RunningInEditor())
    {
        activationState_ = ACTIVATION_VALID;
    }
    else
    {
        UWKProcessCommon::PID apid;
        UWKProcessClient::Instance()->GetActivationServerPID(apid);

        if (apid == UWKProcessClient::Instance()->parentPID_)
        {
            activationState_ = ACTIVATION_VALID;
        }
        else
        {
            ParseKeyFile();

            if (!activationKey_.length())
                activationState_ = ACTIVATION_NEEDKEY;
            else
                activationState_ = ACTIVATION_REQUIRED;
        }
    }

    UWKMessage msg;
    msg.type = UMSG_ACTIVATION_STATE;
    msg.iParams[0] = (int) activationState_;
    UWKMessageQueue::Write(msg);

}

void Activation::DoActivationCheck()
{

}

bool Activation::ActivationRequired()
{
    return activationState_ == ACTIVATION_REQUIRED;
}

QString Activation::GetMachineID()
{
    QString str = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QByteArray hash = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5);
    return QString::fromLatin1(hash.toHex()).toUpper();
}


QString Activation::GetKeyDir()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    path += QString::fromLatin1("/uWebKit");

    QDir dir(path);
    if (!dir.exists())
    {
        QDir mdir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        mdir.mkdir(QString::fromLatin1("uWebKit"));
    }

    return path;

}

QString Activation::GetKeyFilePath()
{
    QString keypath = GetKeyDir() + QString::fromLatin1("/uWebKitKey");
    return keypath;
}



void Activation::RemoveKeyFile()
{
    QString keypath =  GetKeyFilePath();

    if (QFile::exists(keypath))
    {
        QFile::remove(keypath);
    }

}

bool Activation::IsStandardKey(const QString& key)
{
    return key.startsWith(QString::fromLatin1("S"));
}

bool Activation::IsProKey(const QString& key)
{
    return key.startsWith(QString::fromLatin1("P"));
}

bool Activation::ValidateKey(const QString& key)
{
    if (key.length() != 21)
        return false;

    if (!IsProKey(key) && !IsStandardKey(key))
        return false;

    if (key.count(QString::fromLatin1("-")) != 4)
        return false;

    return true;
}

void Activation::WriteKeyFile(const QString& key)
{
    QString keypath =  GetKeyFilePath();

    QFile file(keypath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);

        // activation version
        out << activationVersion_ << '\n';

        out << key << '\n';

        file.close();
    }
}

bool Activation::ParseKeyFile()
{
   QString keypath =  GetKeyFilePath();

   if (!QFile::exists(keypath))
       return false;

   QString paramVersion;
   QString param1;

   QFile file(keypath);
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
       QTextStream in(&file);

       // version
       paramVersion = in.readLine();
       bool ok;
       int version = paramVersion.toInt(&ok);
       if (!ok || version != activationVersion_)
       {
           RemoveKeyFile();
           return false;
       }

       // key
       param1 = in.readLine().trimmed();

       if (!ValidateKey(param1))
       {
           RemoveKeyFile();
           return false;
       }

       activationKey_ = param1;

       return true;
   }

   return false;
}

bool Activation::RunningInEditor()
{
    QDir dir(QCoreApplication::applicationDirPath());

    while (dir.cdUp())
    {
        if (dir.dirName() == QString::fromLatin1("Assets"))
        {
            if (dir.cdUp())
            {
                if (dir.exists(QString::fromLatin1("Library")))
                    return true;
            }
        }
    }

    return false;

}

void Activation::HandleError()
{
    activationState_ = ACTIVATION_ERROR;
    UWKMessage msg;
    msg.type = UMSG_ACTIVATION_STATE;
    msg.iParams[0] = (int) activationState_;
    UWKMessageQueue::Write(msg);

    WriteKeyFile(activationKey_);
}

void Activation::onError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    HandleError();
    request_ = NULL;
}

void Activation::replyFinished(QNetworkReply* reply)
{

    if (request_ == NULL)
        return;

    bool sendMsg = false;

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
        QByteArray bytes = reply->readAll();  // bytes
        QString string = QString::fromLatin1(bytes);

        if (request_->type() == QString::fromLatin1("SUBMITKEY") ||
            request_->type() == QString::fromLatin1("CHECKKEY"))
        {

            bool write = false;
            bool remove = false;

            if (string.contains(QString::fromLatin1("AC_ACTIVATED1")))
            {
                write = true;
                sendMsg = true;
                activationState_ = ACTIVATION_VALID;
            }

            else if (string.contains(QString::fromLatin1("AC_ACTIVATED2")))
            {
                write = true;
                sendMsg = true;
                activationState_ = ACTIVATION_VALID;
            }

            else if (string.contains(QString::fromLatin1("AC_NOACTIVATIONS")))
            {
                activationState_ = ACTIVATION_EXCEEDED;
                sendMsg = true;
                remove = true;
            }

            else if (string.contains(QString::fromLatin1("AC_EXPIRED")))
            {
                activationState_ = ACTIVATION_INVALID;
                sendMsg = true;
                remove = true;
            }
            else if (string.contains(QString::fromLatin1("AC_FAILED_INVALIDKEY")))
            {
                activationState_ = ACTIVATION_INVALID;
                sendMsg = true;
                remove = true;
            }
            // GENERAL FAILURE, catch the specific fail cases above
            else if (string.contains(QString::fromLatin1("AC_FAILED")))
            {
                activationState_ = ACTIVATION_INVALID;
                sendMsg = true;
                remove = true;
            }
            else if (string.contains(QString::fromLatin1("AC_TIMELEFT")))
            {

                activationState_ = ACTIVATION_INVALID;
                sendMsg = true;
                remove = true;
            }
            else
            {
                HandleError();
                return;
            }

            if (write)
                WriteKeyFile(activationKey_);

            if (remove)
                RemoveKeyFile();

        }

    }
    // Some http error received
    else
    {
        HandleError();
        return;
    }

    request_ = NULL;

    if (sendMsg == true)
    {
        UWKMessage msg;
        msg.type = UMSG_ACTIVATION_STATE;
        msg.iParams[0] = (int) activationState_;
        UWKMessageQueue::Write(msg);
    }

}

void Activation::Activate(const QString& key)
{
    if (!ValidateKey(key))
    {
        UWKMessage msg;
        msg.type = UMSG_ACTIVATION_STATE;
        msg.iParams[0] = (int) ACTIVATION_INVALID;
        UWKMessageQueue::Write(msg);
        return;
    }

    if (request_)
        return;

    activationKey_ = key;

    request_ = new ActivationRequest(QString::fromLatin1("SUBMITKEY"));

    connect(request_->manager(), SIGNAL(finished(QNetworkReply*)),
            instance_, SLOT(replyFinished(QNetworkReply*)));

    request_->addPostData(QString::fromLatin1("key"), key);
    request_->addPostData(QString::fromLatin1("id"), GetMachineID());

    request_->loadURL(QUrl(QString::fromLatin1("http://uwk.uwebkit.com/uwk/uwk_activate.php")));

}


}

