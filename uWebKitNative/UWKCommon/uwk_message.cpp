/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#include "uwk_config.h"
#include "uwk_message.h"
#include "Poco/UTF8Encoding.h"
#include <fstream>

UWKMessageQueue* UWKMessageQueue::sWriteQueue_ = NULL;
UWKMessageQueue* UWKMessageQueue::sReadQueue_ = NULL;
uint32_t UWKMessageQueue::sAyncMessageIDCounter_ = 1;

void UWKMessageQueue::Initialize(bool isMainProcess)
{
    //UWKLog::LogVerbose("%lu message queue size\n", UWK_MESSAGE_MAX * sizeof(UWKMessage));

    if (sReadQueue_)
        UWKError::FatalError("Read Queue already defined");

    if (sWriteQueue_)
        UWKError::FatalError("Write Queue already defined");

    if (isMainProcess)
    {
        sReadQueue_ = new UWKMessageQueue(std::string(UWK_MESSAGE_QUEUE_PLUGIN), UWK_READ, true);
        sWriteQueue_ = new UWKMessageQueue(std::string(UWK_MESSAGE_QUEUE_PROCESS), UWK_WRITE, true);
    }
    else
    {
        sWriteQueue_ = new UWKMessageQueue(std::string(UWK_MESSAGE_QUEUE_PLUGIN), UWK_WRITE, false);
        sReadQueue_ = new UWKMessageQueue(std::string(UWK_MESSAGE_QUEUE_PROCESS), UWK_READ, false);
    }

}

void UWKMessageQueue::RecreateQueues()
{
    Shutdown();
    Initialize(true);
}

bool UWKMessageQueue::IsInitialized()
{
    if (!sReadQueue_)
        return false;

    return true;

}

void UWKMessageQueue::Shutdown()
{
    if (sReadQueue_)
        delete sReadQueue_;

    if (sWriteQueue_)
        delete sWriteQueue_;

    sReadQueue_ = sWriteQueue_ = NULL;
}

void UWKMessageQueue::Write(const UWKMessage& message)
{
    if (!sWriteQueue_)
        UWKError::FatalError("Write Queue not defined");

    sWriteQueue_->internalQueue_->send(&message, sizeof(UWKMessage), 0);

}

uint32_t UWKMessageQueue::WriteAsync(UWKMessage& message, UWKAsyncMessageCallback callback)
{
    if (!sWriteQueue_)
        UWKError::FatalError("Write Queue not defined");

    message.asyncMessageId = sAyncMessageIDCounter_++;

    UWKAsyncMessageData asyncData;
    asyncData.asyncMessageId = message.asyncMessageId;
    asyncData.callback = callback;
    sWriteQueue_->asyncMap_.insert(std::pair<uint32_t, UWKAsyncMessageData>(message.asyncMessageId, asyncData));

    sWriteQueue_->internalQueue_->send(&message, sizeof(UWKMessage), 0);

    return message.asyncMessageId;
}

bool UWKMessageQueue::Read(UWKMessage& message)
{
    if (!sReadQueue_)
        UWKError::FatalError("Read Queue not defined");

    memset(&message, 0, sizeof(message));

    message_queue::size_type recvSize;
    unsigned int priority;
    if (sReadQueue_->internalQueue_->try_receive(&message, sizeof(UWKMessage), recvSize,  priority))
    {
        if (message.type == UMSG_ASYNC_RESULT)
        {
            // look in the write queue for the async data
            std::map<uint32_t, UWKAsyncMessageData>::iterator itr = sWriteQueue_->asyncMap_.find(message.asyncMessageId);
            if (itr != sWriteQueue_->asyncMap_.end())
            {
                if (itr->second.callback)
                {
                    std::string value;
                    GetString(message, 0, value);
                    itr->second.callback(message.asyncMessageId, value.c_str());
                }

                sWriteQueue_->asyncMap_.erase(itr);
            }

            // try and read next message
            return Read(message);
        }

        return true;
    }


    return false;
}

bool UWKMessageQueue::GetData(const UWKMessage& msg, int dbIndex, void** data, uint32_t& dataSize)
{
    if (!msg.dataHandle[dbIndex])
    {
        *data = NULL;
        dataSize = 0;
        return false;

    }

    *data = sReadQueue_->dataBuffer_->get_address_from_handle(msg.dataHandle[dbIndex]);
    if (!*data)
    {
        dataSize = 0;
        return false;
    }
    dataSize = msg.dataSize[dbIndex];
    return true;
}

void UWKMessageQueue::Dispose(UWKMessage& msg)
{
    for (size_t i = 0; i < 16; i++)
    {
        if (msg.dataHandle[i])
        {
            void* mem = sReadQueue_->dataBuffer_->get_address_from_handle(msg.dataHandle[i]);
            sReadQueue_->dataBuffer_->deallocate(mem);
        }
    }
}

void UWKMessageQueue::SetString(UWKMessage& msg, int dbIndex, const char *utf8)
{
    // TODO: UTF8 (on the SetString call not locally, should only have UTF8)
    AllocateAndCopy(msg, dbIndex, utf8, strlen(utf8) + 1);
}

void UWKMessageQueue::GetString(const UWKMessage& msg, int dbIndex, std::string& utf8)
{
    char* data;
    uint32_t dataSize;

    if (!GetData(msg, dbIndex, (void**) &data, dataSize) || !dataSize)
    {
        utf8 = "";
        return;
    }

    utf8 = (const char*) data;

}

bool UWKMessageQueue::AllocateAndCopy(UWKMessage& msg, int dbIndex, const void* data, uint32_t dataSize)
{
    void* dest = sWriteQueue_->dataBuffer_->allocate(dataSize, std::nothrow);

    if (!dest)
        return false;

    memcpy(dest, data, dataSize);

    msg.dataHandle[dbIndex] = sWriteQueue_->dataBuffer_->get_handle_from_address(dest);
    msg.dataSize[dbIndex] = dataSize;

    return true;

}

UWKMessageQueue::~UWKMessageQueue()
{
    if (dataBuffer_)
        delete dataBuffer_;

    if (create_)
    {
        message_queue::remove(name_.c_str());
        shared_memory_object::remove(dataBufferName_.c_str());
    }

}

UWKMessageQueue::UWKMessageQueue(const std::string& name, UWKMessageQueueMode mode, bool create):
    mode_(mode),
    dataBuffer_(NULL),
    create_(create)
{
    std::stringstream ss;
    int serverID = UWKConfig::GetServerID();
    std::string prefix;
    UWKConfig::GetSharedMemoryPrefix(prefix);

    if (prefix.length())
        ss << prefix;

    ss << name;
    ss << "_";
    ss << serverID;
    name_ = ss.str();
    ss.str(std::string());

    if (prefix.length())
        ss << prefix;

    ss << name;
    ss << "_databuffer";
    ss << "_";
    ss << serverID;
    dataBufferName_ = ss.str();
    ss.str(std::string());

    if (create)
    {
        //Erase previous message queue
        message_queue::remove(name_.c_str());
        internalQueue_ = new message_queue(create_only, name_.c_str(), UWK_MESSAGE_MAX, sizeof(UWKMessage));

        //Create shared memory
        shared_memory_object::remove(dataBufferName_.c_str());
        dataBuffer_ = new UWKManagedMemory(create_only, dataBufferName_.c_str(), 1024 * 1024 * 4);
    }
    else
    {
        internalQueue_ = new message_queue(open_only, name_.c_str());
        dataBuffer_ = new UWKManagedMemory(open_only, dataBufferName_.c_str());
    }

}

