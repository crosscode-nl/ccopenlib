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
#include <thread>
#include <chrono>

namespace ccol
{
    /**
     * \brief The thread namespace contains classes and functions related to threads or threading.
     */
    namespace thread {

        /** \brief The ThreadPool provides thread pooling functionality.
         *
         *  It uses a pool of std::thread instances internally.
         *
         *  It is written for performance, so be aware that exception handling is not supported.
         *
         *  Throwing an uncaught exception from a job will get std::terminate() to get called
         *  following the defined behavior of std::thread.
         */
        class ThreadPool
        {
        private:
            class Impl;
            std::unique_ptr<Impl> _impl;
        public:

            /** \brief Default constructor
             *
             *  Creates an instance of the threadpool and automatically
             *  allocates an optimal pool of threads. You can check the
             *  amount of thread created by calling the method threadCount()
             */
            ThreadPool();

            /** \brief Default constructor
             *
             *  Creates an instance of the threadpool and automatically
             *  allocates an optimal pool of threads. You can check the
             *  amount of thread created by calling the method threadCount()
             *
             *  \param threadCreateCallback Callback that allow you to perform operations on the std::thread when they are created.
             */
            ThreadPool(const std::function<void(std::thread&)> &threadCreateCallback);

            /** \brief Contructor to create an instance of ThreadPool with a certain amount of thread.
             *
             *  Creates an instance of the threadpool with the specified amount of threads.
             *
             *  Specifying 0 threads will create the optimal amount of threads, just as de default constructor.
             *
             *  \param threads The amount of threads created on the thread pool.
             */
            ThreadPool(const unsigned int &threads);

            /** \brief Contructor to create an instance of ThreadPool with a certain amount of thread.
             *
             *  Creates an instance of the threadpool with the specified amount of threads.
             *
             *  Specifying 0 threads will create the optimal amount of threads, just as de default constructor.
             *
             *  \param threads The amount of threads created on the thread pool.
             *  \param threadCreateCallback Callback that allow you to perform operations on the std::thread when they are created.
             */
            ThreadPool(const unsigned int &threads, const std::function<void(std::thread&)> &threadCreateCallback);

            /** \brief Enqueue a job by copy.
             *
             *  Enqueues a job on the threadpool by making a copy of the passed job.
             *
             *  \param job The job to be executed.
             */
            void enqueue(const std::function<void()> &job);

            /** \brief Enqueue multiple jobs by copy.
             *
             *  Enqueues multiple jobs on the threadpool by making a copy of the passed jobs in the vector.
             *
             *  \param jobs A std::vector containing the jobs to be executed.
             */
            void enqueue(const std::vector<std::function<void()>> &jobs);

            /** \brief Enqueue multiple jobs from queue.
             *
             *  Enqueues multiple jobs on the threadpool by making a copy of the passed jobs in the queue.
             *
             *  The parameter is passed by copy, since the only way to get items out of a queue is to pop them.
             *
             *  This would not be possible with a const reference, and with a reference this would result in
             *  an emty queue.
             *
             *  \param jobs A std::queue containing the jobs to be executed.
             */
            void enqueue(std::queue<std::function<void()>> jobs); // copy because we need to pop...

            /**  \brief Enqueue a job by using move semantics.
             *
             *  Enqueues a job on the threadpool by using move semantics the job to the queue.
             *
             *  \param job The job to be executed.
             */
            void enqueue(std::function<void()> &&job);

            /** \brief Enqueue multiple jobs by using move semantics.
             *
             *  Enqueues multiple jobs on the threadpool by using move semantics the job to the queue.
             *
             *  \param jobs A std::vector containing the jobs to be executed.
             */
            void enqueue(std::vector<std::function<void()>> &&jobs);

            /** \brief Enqueue multiple jobs by using move semantics.
             *
             *  Enqueues multiple jobs on the threadpool by using move semantics.
             *
             *  \param jobs A std::queue containing the jobs to be executed.
             */
            void enqueue(std::queue<std::function<void()>> &&jobs);

            /** Returns the the total amount of jobs. (Currently processing + jobs in queue)
             *
             *  \return Returns the total amount of jobs.
             */
            size_t totalJobCount();

            /** Returns the amount of jobs in the queue.
             *
             *  \return The amount of jobs in the queue.
             */
            size_t queueCount();

            /** \brief Returns the amount of threads in the pool.
             *
             *  \return The amount of threads in the pool.
             */
            unsigned int threadCount() const;

            /**  \brief Removes all jobs from the queue. */
            void clear();

            /**  \brief Removes all jobs from the queue, and returns them for re-adding them later.
             *
             *  This can be used to 'pause' the processing of jobs on the threadpool.
             *  \return A std::queue with the jobs that where pending for execution
             */
            std::queue<std::function<void()>> dequeueAll();

            /**
             * \brief Wait until all jobs are processed.
             */
            void wait();

            /**
            * \brief Wait until all jobs are processed or when a timeout is reached.
            * \param timeout The timeout after when this method should return.
            * \return True when all jobs are processed, false when timeout has been reached.
            */
            bool wait_for(const std::chrono::nanoseconds &timeout);

            /** \brief Wraps the provided job in another lambda function that will execute the job on the provided ThreadPool.
             *
             * The lambda function provided will be wrapped in the returned lambda function that each time it is
             * called will add the provided lambda function to the job queue on the provided threadpool.
             *
             * WARNING: The threadpool must exist when the returned lambda function is invoked, otherwise
             * undefined behavior (a crash, I hope...) is to be expected.
             *
             * The returned lambda could be given to a timer as a callback to create reentrant callbacks.
             *
             * \param job A lambda function to be wrapped.
             * \return The lambda function that will add the job to the job queue when executed.
             */
            std::function<void()> wrap(const std::function<void()> &job);

            /** \brief The destructor
             *
             *  Destructing the threadpool will lead to the std::thread to be stopped and
             *  joined. Any job that is still executing will block destruction until it is
             *  completed. Therefore it is adviced if you add long running jobs, that you
             *  add some cancelation mechanism.
             */
            virtual ~ThreadPool();
        };
    }
}

#endif // CCOPENLIB_THREADPOOL_H
