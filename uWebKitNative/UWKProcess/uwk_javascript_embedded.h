/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <QString>

namespace UWK {

class JavascriptEmbedded
{
    static QString sIMEHandler_;
    static QString sBridgeJS_;

    static void initialzeIMEHandler();
    static void initialzeBridgeJS();

public:

    static void Initialize();

    static const QString& GetIMEHandler() { return sIMEHandler_; }
    static const QString& GetBridgeJS() { return sBridgeJS_; }

};


}
