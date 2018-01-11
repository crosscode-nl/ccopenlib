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
#ifndef CCOL_THREAD_CCOPENLIB_TIMER_H
#define CCOL_THREAD_CCOPENLIB_TIMER_H

#include <memory>
#include <vector>
#include <queue>
#include <functional>
#include <chrono>

namespace ccol
{
    namespace thread
    {
        /** \brief The Timer executes a callback after a certain amount of time.
         *
         * It can execute at a certain interval or once after a delay.
         *
         * It does execute the callback from a thread but it will wait for the callback
         * to finish before it starts the next pending callback when its execution takes
         * longer than the interval.
         *
         * If you need to have the next callback called when the previous is not finished
         * you should create a wrapper callback that places the job on a thread pool or
         * starts the work in (detached) std::thread.
         *
         * Throwing an uncaught exception from the callback will get std::terminate()
         * to get called following the defined behavior of std::thread.
         */
        class Timer
        {
        private:
            class Impl;
            std::unique_ptr<Impl> _impl;
        public:
            /** \brief Default constructor */
            Timer();

            /** \brief Constructor that accepts a callback by const reference.
             *
             * \param callback The const reference of the callback.
             */
            Timer(const std::function<void()> &callback);

            /** \brief Constructor that accepts a callback using move semantics.
             *
             * \param callback The move assignment of the callback.
             */
            Timer(std::function<void()> &&callback);


            /** \brief Sets the reliablity of the timer
             *
             * Improved the reliability of the timer by staring a busy_loop. Busy loops are expensive. Leave 0 is you can live with some inaccuracy.
             *
             * \param reliability Reliability parameter is used to determine the (aporximate) time before the expiration to start the busy loop. Use a value between 0 and 1000.
             */
            void setReliability(const unsigned int &reliability);

            /** \brief Start the timer with an interterval after an initial delay.
             *
             *  Use std::chrono::duration to set the initial delay and interval. For example std::chrono::milliseconds(1) or std::chrono::seconds(5)
             *
             *  To start immediately use a delay of 0.
             *
             *  \param delay The initial delay in std::chrono::nanoseconds.
             *  \param interval The interval in std::chrono::nanoseconds.
             */
            void start(const std::chrono::nanoseconds& delay, const std::chrono::nanoseconds& interval);

            /** \brief Start the timer with an interterval.
             *
             *  Use std::chrono::duration to set the interval. For example std::chrono::milliseconds(1) or std::chrono::seconds(5)
             *
             *  This will start timer with a delay of interval.
             *
             *  \param interval The initial delay and interval in std::chrono::nanoseconds.
             */
            void start(const std::chrono::nanoseconds& interval);

            /** \brief Fire the timer once after a delay.
             *
             *  Use std::chrono::duration to set the delay. For example std::chrono::milliseconds(1) or std::chrono::seconds(5)
             *
             *  This will start timer with a delay of interval.
             *
             *  \param delay The delay in std::chrono::nanoseconds.
             */
            void startSingleshot(const std::chrono::nanoseconds& delay);

            /** \brief Set the callback by const reference.
             *
             *  \param callback The const reference of the callback.
             */
            void setCallback(const std::function<void()> &callback);

            /** \brief Set the callback using move semantics.
             *
             *  \param callback The move assignment of the callback.
             */
            void setCallback(std::function<void()> &&callback);

            /**  \brief Stop the timer. */
            void stop();

            /**  \brief Stop the timer.
             *
             *  Destructing the timer will lead to the std::thread to be stopped and
             *  joined. Any job that is still executing will block destruction until it is
             *  completed. Therefore it is adviced if you add long running callbacks, that you
             *  add some cancelation mechanism.
             */
            virtual ~Timer();
        };
    }
}

#endif // CCOL_THREAD_CCOPENLIB_TIMER_H
