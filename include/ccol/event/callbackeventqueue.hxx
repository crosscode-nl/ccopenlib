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
#ifndef CCOL_EVENT_CALLBACKEVENTQUEUE_HXX
#define CCOL_EVENT_CALLBACKEVENTQUEUE_HXX
#include <ccol/event/eventqueue.hxx>
#include <functional>
#include <utility>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>


namespace ccol {
    namespace event {

        /**
         * \brief The CallbackEventQueue class processes callback events by executing them.
         *
         * This is a specialized EventQueue that executes lambda functions on the thread
         * the event queue is running on.
         */
        class CallbackEventQueue
        {
            private:
            EventQueue _eventQueue;
            public:

            /**
             * \brief The default constructor of the CallbackEventQueue.
             */
            CallbackEventQueue();

            /**
             * \brief Constructor of the CallbackEventQueue that allows setting a queue limitation.
             * \param maxQueueSize The maximum items allowed in the queue.
             *
             * When the queue is full, enqueue will return false.
             */
            CallbackEventQueue(const std::size_t &maxQueueSize);

            /**
             * \brief Enqueue lambda function on event queue.
             * \param function The lambda function to execute on the thread where the event queue is running.
             * \return True when lambda function is queued, false when queue is full.
             */
            bool enqueue(const std::function<void()> &function);

            /**
             * \brief Enqueue lambda function on event queue using move semantics.
             * \param function The lambda function to execute on the thread where the event queue is running.
             * \return True when lambda function is queued, false when queue is full.
             */
            bool enqueue(std::function<void()> &&function);

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

            /** \brief Wraps the provided job in another lambda function that will execute the job on the provided EventQueue.
             *
             * The lambda function provided will be wrapped in the returned lambda function that each time it is
             * called will add the provided lambda function to the job queue on the provided EventQueue.
             *
             * WARNING: The EventQueue must exist when the returned lambda function is invoked, otherwise
             * undefined behavior (a crash, I hope...) is to be expected.
             *
             * The returned lambda could be given to a timer as a callback to create callbacks
             * that will run on the same thread that the EventQueue is running on.
             *
             * \param job A lambda function to be wrapped.
             * \return The lambda function that will add the job to the EventQueue when executed.
             */
            std::function<bool()> wrap(const std::function<void()> &job);

            /**
             * @brief virtual destructor ~CallbackEventQueue
             */
            virtual ~CallbackEventQueue();
        };

    }
}
#endif
