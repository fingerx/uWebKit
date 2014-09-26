/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include <string>
#include <QString>

#include "UWKCommon/uwk_message.h"

namespace UWK {

class QtUtils
{
    public:

        static QString GetMessageQString(const UWKMessage& msg, int index)
        {
            std::string str;
            UWKMessageQueue::GetString(msg, index, str);
            return QString::fromUtf8(str.c_str());
        }

        static void SetMessageQString(UWKMessage& msg, int index, const QString& string)
        {
            UWKMessageQueue::SetString(msg, index, string.toUtf8().constData());
        }

};

}
