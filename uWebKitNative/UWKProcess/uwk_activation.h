

/******************************************
  * uWebKit
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder
  * for details
*******************************************/

#pragma once

#include <QtCore>
#include <QString>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "UWKCommon/uwk_message.h"
#include "uwk_activationrequest.h"


namespace UWK
{

class Activation : public QObject
{

    Q_OBJECT

    static ActivationState activationState_;
    static int activationVersion_;
    static QString activationKey_;

    static Activation* instance_;

    static ActivationRequest* request_;

    static bool RunningInEditor();
    static QString GetMachineID();
    static QString GetKeyDir();
    static QString GetKeyFilePath();

    static bool ValidateKey(const QString& key);
    static bool IsStandardKey(const QString& key);
    static bool IsProKey(const QString& key);

    static bool ParseKeyFile();
    static void RemoveKeyFile();
    static void WriteKeyFile(const QString& key);
    void HandleError();

public:

    static void Initialize();

    static bool ActivationRequired();

    static void Activate(const QString& key);

    static void DoActivationCheck();

    static ActivationState GetActivationState() { return activationState_; }

public slots:

    void replyFinished(QNetworkReply* reply);

    void onError(QNetworkReply::NetworkError code);


};


}
