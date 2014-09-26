/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

#include "Poco/Timestamp.h"
using Poco::Timestamp;

#ifdef _MSC_VER
// Poco::Timestamp include seems have a min issue on Windows
#undef min
#endif

#include <stdlib.h>
#include <map>

#include "uwk_common.h"
#include "uwk_interprocess.h"

using namespace boost::interprocess;

#ifdef _MSC_VER
typedef managed_windows_shared_memory UWKManagedMemory;
#else
typedef managed_shared_memory UWKManagedMemory;
#endif

enum UWKMessageType
{
    UMSG_INVALID,
    UMSG_VIEW_CREATE,
    UMSG_VIEW_DESTROY,
    UMSG_GPUSURFACE_INFO,
    UMSG_MOUSE_MOVE,
    UMSG_MOUSE_DOWN,
    UMSG_MOUSE_UP,
    UMSG_MOUSE_SCROLL,
    UMSG_KEY_DOWN,
    UMSG_KEY_UP,
    UMSG_LOG,
    UMSG_ERROR,
    UMSG_VIEW_LOADFINISHED,
    UMSG_VIEW_URLCHANGED,
    UMSG_VIEW_TITLECHANGED,
    UMSG_VIEW_ICONCHANGED,
    UMSG_VIEW_CONTENTSIZECHANGED,
    UMSG_VIEW_SETCURRENTSIZE,
    UMSG_VIEW_SETSCROLLPOSITION,
    UMSG_VIEW_EVALUATE_JAVASCRIPT,
    UMSG_VIEW_LOADURL,
    UMSG_VIEW_LOADHTML,
    UMSG_VIEW_SHOW,
    UMSG_VIEW_NAVIGATE,
    UMSG_VIEW_SETZOOMFACTOR,
    UMSG_VIEW_REQUESTNEWVIEW,
    UMSG_VIEW_SETALPHAMASK,
    UMSG_VIEW_SETTEXTCARETCOLOR,
    UMSG_VIEW_RELOAD,
    UMSG_VIEW_SETFRAMERATE,
    UMSG_VIEW_SETUSERAGENT,
    UMSG_ASYNC_RESULT,
    UMSG_JAVASCRIPT_CONSOLE,
    UMSG_JAVASCRIPT_MESSAGE,
    UMSG_JSOBJECT_SETPROPERTY,
    UMSG_JSOBJECT_REMOVE,
    UMSG_VIEW_LOADPROGRESS,
    UMSG_SHUTDOWN_WEBRENDERPROCESS,
    UMSG_COOKIES_CLEAR,
    UMSG_IME_FOCUSIN,
    UMSG_IME_FOCUSOUT,
    UMSG_IME_SETTEXT,
    UMSG_DEV_CRASHWEBPROCESS,
    UMSG_DEV_HANGWEBPROCESS,
    UMSG_ACTIVATION_STATE,
    UMSG_ACTIVATE
};

enum ActivationState
{
    ACTIVATION_CHECK_REQUIRED,
    ACTIVATION_VALID,
    ACTIVATION_INVALID,
    ACTIVATION_NEEDKEY,
    ACTIVATION_EXCEEDED,
    ACTIVATION_ERROR
};

typedef void (*UWKAsyncMessageCallback)(uint32_t id, const char* utf8Value);

struct UWKAsyncMessageData
{
    uint32_t asyncMessageId;

    UWKAsyncMessageCallback callback;

    // for detecting timeouts
    Timestamp timestamp;
};

struct UWKMessage
{
    UWKMessage()
    {
        // _this is set in C#
        _this = NULL;

        type = UMSG_INVALID;
        browserID = -1;
        asyncMessageId = 0;
        memset(dataHandle, 0, sizeof(UWKManagedMemory::handle_t) * 16);
        memset(dataSize, 0, sizeof(uint32_t) * 16);
    }

    // so we can query from C#
    UWKMessage* _this;

    UWKMessageType type;

    uint32_t asyncMessageId;

    uint32_t browserID;

    // integer parameters
    int32_t iParams[16];

    // float parameters
    float fParams[16];

    uint32_t dataSize[16];    
    UWKManagedMemory::handle_t dataHandle[16];

};

enum UWKMessageQueueMode
{
    UWK_WRITE,
    UWK_READ
};

class UWKMessageQueue
{
    // internal message queue
    message_queue* internalQueue_;

    // message queue name
    std::string name_;
    UWKMessageQueueMode mode_;

    std::string dataBufferName_;
    UWKManagedMemory* dataBuffer_;

    std::map<uint32_t, UWKAsyncMessageData> asyncMap_;

    bool create_;

    static UWKMessageQueue* sWriteQueue_;
    static UWKMessageQueue* sReadQueue_;
    static uint32_t sAyncMessageIDCounter_;

    UWKMessageQueue(const std::string& name, UWKMessageQueueMode mode, bool create = false);
    ~UWKMessageQueue();

public:

    static void Initialize(bool isMainProcess = false);
    static void Shutdown();

    static bool IsInitialized();

    static bool AllocateAndCopy(UWKMessage& msg, int dbIndex, const void* data, uint32_t dataSize);
    static bool GetData(const UWKMessage& msg, int dbIndex, void** data, uint32_t& dataSize);

    static void SetString(UWKMessage& msg, int dbIndex, const char* utf8);
    static void GetString(const UWKMessage& msg, int dbIndex, std::string& utf8);

    static void Dispose(UWKMessage& msg);

    static void Write(const UWKMessage& message);
    static uint32_t WriteAsync(UWKMessage& message, UWKAsyncMessageCallback callback);

    static void RecreateQueues();

    static bool Read(UWKMessage& message);

};
