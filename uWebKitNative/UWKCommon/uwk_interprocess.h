/******************************************
  * uWebKit 
  * (c) 2014 THUNDERBEAST GAMES, LLC
  * website: http://www.uwebkit.com email: sales@uwebkit.com
  * Usage of this source code requires a uWebKit Source License
  * Please see UWEBKIT_SOURCE_LICENSE.txt in the root folder 
  * for details
*******************************************/

#pragma once

// THIS SHOULD BE THE ONLY PLACE WE'RE INCLUDING BOOST INTERPROCESS STUFF!

#define UWK_MESSAGE_MAX 1024
#define UWK_MESSAGE_QUEUE_PLUGIN "uwk_message_queue_plugin"
#define UWK_MESSAGE_QUEUE_PROCESS "uwk_message_queue_process"

// workaround for 32 bit compile (check for 64 bit)
namespace boost {
    namespace interprocess {
        namespace ipcdetail {

            //Rounds "orig_size" by excess to round_to bytes
            template<class SizeType, class ST2>
            inline SizeType get_rounded_size(SizeType orig_size, ST2 round_to)
            {
                return ((orig_size-1)/round_to+1)*round_to;
            }
        }
    }
}

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#ifndef _MSC_VER
    #include <boost/interprocess/managed_shared_memory.hpp>
#else
    #include <boost/interprocess/managed_windows_shared_memory.hpp>
#endif

#include <iostream>
#include <vector>

