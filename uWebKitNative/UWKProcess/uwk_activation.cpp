
#include "UWKCommon/uwk_config.h"
#include "UWKCommon/uwk_process_client.h"
#include "uwk_activation.h"


namespace UWK
{

ActivationState Activation::activationState_ = ACTIVATION_REQUIRED;
int Activation::activationVersion_ = 1;
QString Activation::activationKey_;

void Activation::Initialize()
{

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


}

