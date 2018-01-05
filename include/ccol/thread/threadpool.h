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
#ifndef CCOPENLIB_THREADPOOL_H
#define CCOPENLIB_THREADPOOL_H

#include <memory>
#include <vector>
#include <queue>
#include <functional>

namespace ccol
{
    namespace thread {

        ///	The ThreadPool class provides threadpool functionality.
        ///
        /// It uses a pool of std::thread instances internally.
        ///
        /// It is written for performance, so be aware that exception handling is not supported.
        ///
        /// Throwing an uncaught exception from a job will get std::terminate to get called
        /// following the defined behavior of std::thread.
        class ThreadPool
        {
        private:
            class Impl;
            std::unique_ptr<Impl> _impl;
        public:

            /// \brief Default constructor
            ///
            /// Creates an instance of the threadpool and automatically
            /// allocates an optimal pool of threads. You can check the
            /// amount of thread created by calling the method threadCount()
            ThreadPool();

            /// \brief Contructor to create an instance of ThreadPool with a certain amount of thread.
            ///
            /// Creates an instance of the threadpool with the specified amount of threads.
            ///
            /// Specifying 0 threads will create the optimal amount of threads, just as de default constructor.
            ///
            /// \param threads The amount of threads created on the thread pool.
            ThreadPool(const unsigned int &threads);

            /// \brief Enqueue a job by copy.
            ///
            /// Enqueues a job on the threadpool by making a copy of the passed method.
            ///
            /// \param method The method to be executed.
            void enqueueJob(const std::function<void()> &method);

            /// \brief Enqueue multiple jobs by copy.
            ///
            /// Enqueues multiple jobs on the threadpool by making a copy of the passed methods in the vector.
            ///
            /// \param methods A std::vector containing the methods to be executed.
            void enqueueJob(const std::vector<std::function<void()>> &methods);

            /// \brief Enqueue multiple jobs from queue.
            ///
            /// Enqueues multiple jobs on the threadpool by making a copy of the passed methods in the queue.
            ///
            /// The parameter is passed by copy, since the only way to get items out of a queue is to pop them.
            ///
            /// This would not be possible with a const reference, and with a reference this would result in
            /// an emty queue.
            ///
            /// \param methods A std::queue containing the methods to be executed.
            void enqueueJob(std::queue<std::function<void()>> methods); // copy because we need to pop...

            /// \brief Enqueue a job by using move semantics.
            ///
            /// Enqueues a job on the threadpool by using move semantics the method to the queue.
            ///
            /// \param method The method to be executed.
            void enqueueJob(std::function<void()> &&method);

            /// \brief Enqueue multiple jobs by using move semantics.
            ///
            /// Enqueues multiple jobs on the threadpool by using move semantics the method to the queue.
            ///
            /// \param methods A std::vector containing the methods to be executed.
            void enqueueJob(std::vector<std::function<void()>> &&methods);

            /// \brief Enqueue multiple jobs by using move semantics.
            ///
            /// Enqueues multiple jobs on the threadpool by using move semantics the method to the queue.
            ///
            /// \param methods A std::queue containing the methods to be executed.
            void enqueueJob(std::queue<std::function<void()>> &&methods);

            /// Returns the amount of jobs in the queue.
            ///
            /// \return The amount of jobs in the queue.
            size_t jobsInQueueCount();

            /// \brief Returns the amount of threads in the pool.
            ///
            /// \return The amount of threads in the pool.
            unsigned int threadCount();

            /// Removes all jobs from the queue.
            void clearQueue();

            /// \brief Removes all jobs from the queue, and returns them for re-adding them later.
            ///
            /// This can be used to 'pause' the processing of jobs on the threadpool.
            /// \return A std::queue with the jobs that where pending for execution
            std::queue<std::function<void()>> pullJobsFromQueue();

            /// \brief The destructor
            ///
            /// Destructing the threadpool will lead to the std::thread to be stopped and
            /// joined. Any job that is still executing will block destruction until it is
            /// completed. Therefor it is adviced if you add long running jobs, that you
            /// add some cancelation mechanism.
            virtual ~ThreadPool();
        };
    }
}

#endif // CCOPENLIB_THREADPOOL_H
