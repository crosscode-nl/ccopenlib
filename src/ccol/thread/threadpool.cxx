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
#include <ccol/thread/threadpool.hxx>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>

namespace ccol
{
    namespace thread {

        class ThreadPool::Impl
        {
        private:
            unsigned int _threadCount = 0;
            std::vector<std::thread> _threads;
            std::condition_variable _threadUnlocked;
            std::queue<std::function<void()>> _jobs;
            std::mutex _jobsMutex;
            volatile bool _running = true;
            void threadSpinner();
            inline std::function<void()> threadRetrieveCallback();
            inline void lockedEnqueue(const std::vector<std::function<void()>> &jobs);
            inline void lockedEnqueue(const std::function<void()> &job);
            inline void lockedEnqueue(std::vector<std::function<void()>> &&jobs);
            inline void lockedEnqueue(std::queue<std::function<void()>> &&jobs);
            inline void lockedEnqueue(std::function<void()> &&job);
        public:
            Impl(const unsigned int &threads, const std::function<void(std::thread&)> &threadCreateCallback);
            inline void enqueue(const std::function<void()> &job);
            inline void enqueue(const std::vector<std::function<void()>> &jobs);
            inline void enqueue(std::function<void()> &&job);
            inline void enqueue(std::vector<std::function<void()>> &&jobs);
            inline void enqueue(std::queue<std::function<void()>> &&jobs);
            inline size_t queueCount();
            inline unsigned int threadCount() const;
            inline void clear();
            inline std::queue<std::function<void()>> dequeueAll();
            ~Impl();
        };

        inline void ThreadPool::Impl::clear()
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            _jobs = std::queue<std::function<void()>>();
        }

        inline std::queue<std::function<void()>> ThreadPool::Impl::dequeueAll()
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            std::queue<std::function<void()>> result = std::move(_jobs);
            _jobs = std::queue<std::function<void()>>(); // create new empty queue/
            return result;
        }

        unsigned int ThreadPool::Impl::threadCount() const
        {
            return _threadCount;
        }

        size_t ThreadPool::Impl::queueCount()
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            return _jobs.size();
        }

        ThreadPool::Impl::Impl(const unsigned int &threads, const std::function<void(std::thread&)> &threadCreateCallback)
        {
            if (threads == 0) {
                _threadCount = std::thread::hardware_concurrency();
            }
            else {
                _threadCount = threads;
            }
            if (_threadCount == 0) {
                _threadCount = 1;
            }
            for (unsigned int idx = 0; idx < _threadCount; idx++) {
                std::thread thread = std::thread(&Impl::threadSpinner, this);
                if (threadCreateCallback!=nullptr) {
                    threadCreateCallback(thread);
                }
                _threads.push_back(std::move(thread));
            }
        }

        std::function<void()> ThreadPool::Impl::threadRetrieveCallback()
        {
            std::function<void()> callback = nullptr;
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            _threadUnlocked.wait(jobsMutexLock, [this]() { return !_jobs.empty() || !_running; });
            if (!_running) return nullptr;
            callback = std::move(_jobs.front());
            _jobs.pop();
            return callback;
        }

        void ThreadPool::Impl::threadSpinner()
        {
            while (_running) {
                std::function<void()> callback(threadRetrieveCallback());
                if (_running && callback != nullptr) {
                    callback();
                }
            }
        }

        void ThreadPool::Impl::lockedEnqueue(const std::vector<std::function<void()>> &jobs)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            for (const auto &job : jobs) {
                _jobs.push(job);
            }
        }


        void ThreadPool::Impl::lockedEnqueue(const std::function<void()> &job)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            _jobs.push(job);
        }

        void ThreadPool::Impl::lockedEnqueue(std::vector<std::function<void()>> &&jobs)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            for (const auto &job : jobs) {
                _jobs.push(std::move(job));
            }
        }

        void ThreadPool::Impl::lockedEnqueue(std::queue<std::function<void()>> &&jobs)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            while (!jobs.empty()) {
                _jobs.push(std::move(jobs.front()));
                jobs.pop();
            }
        }

        void ThreadPool::Impl::lockedEnqueue(std::function<void()> &&job)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex );
            _jobs.push(std::move(job));
        }

        void ThreadPool::Impl::enqueue(const std::function<void()> &job)
        {
            lockedEnqueue(job);
            _threadUnlocked.notify_one(); // since one job is added, wake up one extra threads.
        }

        void ThreadPool::Impl::enqueue(const std::vector<std::function<void()>> &jobs)
        {
            lockedEnqueue(jobs);
            _threadUnlocked.notify_all(); // multiple jobs are added, wake up all threads.
        }


        void ThreadPool::Impl::enqueue(std::function<void()> &&job)
        {
            lockedEnqueue(std::move(job));
            _threadUnlocked.notify_one(); // since one job is added, wake up one extra threads.
        }

        void ThreadPool::Impl::enqueue(std::vector<std::function<void()>> &&jobs)
        {
            lockedEnqueue(std::move(jobs));
            _threadUnlocked.notify_all(); // multiple jobs are added, wake up all threads.
        }

        void ThreadPool::Impl::enqueue(std::queue<std::function<void()>> &&jobs)
        {
            lockedEnqueue(std::move(jobs));
            _threadUnlocked.notify_all(); // multiple jobs are added, wake up all threads.
        }

        ThreadPool::Impl::~Impl()
        {
            _running = false;
            { // scope to release the lock.
                std::unique_lock<std::mutex> jobsMutexLock( _jobsMutex); // acquire lock, otherwise not all threads are waiting...
                _threadUnlocked.notify_all();
            }

            for (std::thread &thread : _threads) {
                thread.join();
            }
        }

        ThreadPool::ThreadPool()
            : _impl(std::make_unique<Impl>(0,nullptr))
        {
        }

        ThreadPool::ThreadPool(const std::function<void (std::thread &)> &threadCreateCallback)
        : _impl(std::make_unique<Impl>(0,threadCreateCallback))
        {
        }

        ThreadPool::ThreadPool(const unsigned int &threads)
            : _impl(std::make_unique<Impl>(threads,nullptr))
        {
        }

        ThreadPool::ThreadPool(const unsigned int &threads, const std::function<void(std::thread&)> &threadCreateCallback)
            : _impl(std::make_unique<Impl>(threads,threadCreateCallback))
        {
        }

        void ThreadPool::enqueue(const std::function<void()> &job)
        {
            _impl->enqueue(job);
        }

        void ThreadPool::enqueue(const std::vector<std::function<void()>> &jobs)
        {
            _impl->enqueue(jobs);
        }

        void ThreadPool::enqueue(std::queue<std::function<void()>> jobs)
        {
            _impl->enqueue(std::move(jobs));
        }

        void ThreadPool::enqueue(std::function<void()> &&job)
        {
            _impl->enqueue(std::move(job));
        }

        void ThreadPool::enqueue(std::vector<std::function<void()>> &&jobs)
        {
            _impl->enqueue(std::move(jobs));
        }

        void ThreadPool::enqueue(std::queue<std::function<void()>> &&jobs)
        {
            _impl->enqueue(std::move(jobs));
        }

        size_t ThreadPool::queueCount()
        {
            return _impl->queueCount();
        }

        unsigned int ThreadPool::threadCount() const
        {
            return _impl->threadCount();
        }

        void ThreadPool::clear()
        {
            return _impl->clear();
        }

        std::queue<std::function<void()>> ThreadPool::dequeueAll()
        {
            return _impl->dequeueAll();
        }

        std::function<void ()> ThreadPool::wrapper(const std::function<void ()> &job)
        {
            return [this, job]{
                enqueue(job);
            };
        }

        ThreadPool::~ThreadPool()
        {
        }
    }
}
