/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "UWKCommon/uwk_log.h"
#include "uwk_javascript_embedded.h"

namespace UWK
{

QString JavascriptEmbedded::sIMEHandler_;

QString JavascriptEmbedded::sBridgeJS_;

void JavascriptEmbedded::initialzeBridgeJS()
{
    // note that js here needs ; as will be on same line

    sBridgeJS_ = QString::fromLatin1("UWK.callbacks = {};\
    UWK.sendMessage = function(msgName, msgValue)\
    {\
        msgValue = JSON.stringify(msgValue);\
        UWK._sendMessage(UWK.viewId, msgName, msgValue);\
    };\
    UWK.subscribe = function(msgName, callback)\
    {\
        var callbacks = UWK.callbacks[msgName];\
        if (callbacks === undefined)\
        {\
            callbacks = UWK.callbacks[msgName] = [];\
        }\
        if (callbacks.indexOf(callback) != -1)\
            return;\
        callbacks.push(callback);\
    };\
    UWK.messageReceived.connect ( function(msgName, msgString)\
    {\
        var callbacks = UWK.callbacks[msgName];\
        if (callbacks === undefined)\
            return;\
        var msgObject = JSON.parse(msgString);\
        for (var i = 0; i < callbacks.length; i++) \
        {\
            callbacks[i](msgName, msgObject);\
        }\
});\
");
}

void JavascriptEmbedded::initialzeIMEHandler()
{
    sIMEHandler_ = QString::fromLatin1("function UWK_isTextElement(el) { \
                                                               return el.tagName.toLowerCase() == \"input\" && (el.type.toLowerCase() == \"text\" || el.type.toLowerCase() == \"password\"); \
} \
UWK_TextElement = 0; \
function UWK_setTextElement(value) { \
    UWK_TextElement.value = value; \
} \
this.addEventListener(\"DOMFocusIn\", function(e) { \
                       if (UWK_isTextElement(e.target)) { \
                       UWK_TextElement = e.target; \
var rectObject = UWK_TextElement.getBoundingClientRect(); \
UWK_View.handleFocusIn(e.target.type.toLowerCase(), e.target.value, rectObject.left, rectObject.top, rectObject.width, rectObject.height); \
} \
}, false); \
this.addEventListener(\"DOMFocusOut\", function(e) { \
                       if (UWK_isTextElement(e.target)) { \
                       UWK_View.handleFocusOut(e.target.value); \
UWK_TextElement = 0; \
} \
}, false)");

}

void JavascriptEmbedded::Initialize()
{
    initialzeIMEHandler();
    initialzeBridgeJS();
}



}
