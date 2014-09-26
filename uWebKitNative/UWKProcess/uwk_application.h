/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <QApplication>

namespace UWK
{


class UWKApplication : public QApplication
{
    Q_OBJECT

    void PumpMessages();

    static void LogCallback(const char* message, int level);
    static void ErrorCallback(const char* message, bool fatal);

    int timerId_;

public:

    UWKApplication(int &argc, char **argv);

    void init();
    void shutdown();

signals:

public slots:
    void update();

private:

    void timerEvent(QTimerEvent *event);

};

}
