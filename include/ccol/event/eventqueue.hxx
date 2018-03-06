/*
    SPDX-License-Identifier: MIT

    © 2017 CrossCode / Patrick Vollebregt - All rights reserved

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    If you use this code, please mention usages of this library and the copyright notice visible
    in your end product or distributed documentation. For example:

    This product uses "ccopenlib" written and copyrighted by CrossCode / Patrick Vollebregt.
    Visit http://www.ccopenlib.com for more information.

    If for some reason this not possible, please contact: ccopenlib@crosscode.nl to purchase a license exception.

    If you'd like to modify and/or share this code, share it under the same license, and keep the original copyright notice intact.

    If you have found any errors or improvements you'd like to share, please contact me: ccopenlib@crosscode.nl
*/
#ifndef CCOL_EVENT_EVENTQUEUE_HXX
#define CCOL_EVENT_EVENTQUEUE_HXX
#include <ccol/event/baseevent.hxx>
#include <functional>
#include <utility>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>


namespace ccol {
    namespace event {
        /**
         * \brief EventQueue implementation that allows cross-thread event messaging.
         *
         * All methods are thread safe.
         *
         * All messages will be passed to the event handlers registered with setCallbackForType
         * or setCallbacks when the event loop is started with run(). To stop the event loop call
         * stop() from any thread.
         */
        class EventQueue
        {

        private:
            class Impl;
            std::unique_ptr<Impl> _impl;

        public:
            /**
             * \brief event_type is a share_ptr to the base type of all events.
             */
            typedef std::shared_ptr<BaseEvent> event_type;

            /**
             * \brief callback_type is the type of your event handler.
             */
            typedef std::function<void(event_type&&)> callback_type;

            /**
             * \brief callback_vector_type is the type of a collection of event handlers.
             */
            typedef std::vector<std::pair<std::type_index,callback_type>> callback_vector_type;

            /**
             * \brief The default constructor of the EventQueue.
             */
            EventQueue();

            /**
             * \brief Constructor of the EventQueue that allows setting a queue limitation.
             * \param maxQueueSize The maximum items allowed in the queue.
             *
             * When the queue is full, enqueue will return false.
             */
            EventQueue(const std::size_t &maxQueueSize);

            /**
             * \brief enqueue an event that is of type event_type.
             * \param event The event to queue.
             * \return true when enqueue is successful, false when queue is full.
             *
             * Enqueue an event of type event_type.
             *
             * This is a shared_ptr to types inherited from BaseEvent.
             *
             * The following types are already available:
             *
             * StaticDataEvent<T>()
             * SharedDataEvent<T>()
             * CallbackEvent()
             *
             * Use the data events to message data.
             * Use the callback event to message a callback.
             */
            bool enqueue(const event_type &event);

            /**
             * \brief enqueue an event that is of type event_type by using move semantics.
             * \param event The event to queue.
             * \return true when enqueue is successful, false when queue is full.
             *
             * Enqueue an event of type event_type.
             *
             * This is a shared_ptr to types inherited from BaseEvent.
             *
             * The following types are already available:
             *
             * StaticDataEvent<T>()
             * SharedDataEvent<T>()
             * CallbackEvent()
             *
             * Use the data events to message data.
             * Use the callback event to message a callback.
             */
            bool enqueue(event_type &&event);

            /**
             * \brief setCallbackForType adds a lambda function as a handler for an event of type type.
             *
             *
             *
             * \param type The type of the event the callback should handle.
             * \param callback A lambda function that handles the event.
             */
            void setCallbackForType(const std::type_index &type, const callback_type &callback);

            /**
             * \brief setCallbackForType adds a lambda function using move semantics as a handler for an event of type type.
             *
             *    typedef ccol::event::StaticDataEvent<int> IntEvent;
             *    queue.setCallbackForType(typeid(IntEvent),[&queue,&intCallbackCalled](ccol::event::EventQueue::event_type &&event){
             *       auto dataEvent = std::dynamic_pointer_cast<IntEvent>(event);
             *       // handle event
             *    });
             *
             * \param type The type of the event the callback should handle.
             * \param callback A lambda function that handles the event.
             */
            void setCallbackForType(const std::type_index &type, callback_type &&callback);

            /**
             * \brief setCallbacks adds callbacks from a vector containing std::pair<std::type_index,callback_type> combinations.
             * \param callbacks The vector containing std::pair<std::type_index,callback_type>
             */
            void setCallbacks(const callback_vector_type &callbacks);

            /**
             * \brief setCallbacks adds callbacks from a vector containing std::pair<std::type_index,callback_type> combinations using move semantics.
             * \param callbacks The vector containing std::pair<std::type_index,callback_type>
             */
            void setCallbacks(callback_vector_type &&callbacks);

            /**
             * \brief run starts processing the event queue.
             *
             * Starts processing the event queue. This method exits when stop() is called.
             */
            void run();

            /**
             * \brief isRunning returns the running state.
             * \return true when the event queue is running.
             */
            bool isRunning();

            /**
             * \brief stop the event queue when it is running.
             *
             * The run method will return after this call.
             */
            void stop();

            /**
             * \brief The virtual destructor ~EventQueue
             */
            virtual ~EventQueue();
        };

    }
}
#endif
