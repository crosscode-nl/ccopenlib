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
            std::condition_variable _totalReducedCountCv;
            size_t _totalJobsCount = 0;
            std::condition_variable _jobsCv;
            std::vector<std::thread> _threads;
            std::queue<std::function<void()>> _jobs;
            std::mutex _stateMutex;
            volatile bool _running = true;
            void threadSpinner();
            inline std::function<void()> threadRetrieveCallback(const bool &reduceJobs);
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
            inline size_t totalJobCount();
            inline unsigned int threadCount() const;
            inline void clear();
            inline std::queue<std::function<void()>> dequeueAll();
            void wait();
            bool wait_for(const std::chrono::nanoseconds &timeout);
            ~Impl();
        };

        inline void ThreadPool::Impl::clear()
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
            _jobs = std::queue<std::function<void()>>();
        }

        inline std::queue<std::function<void()>> ThreadPool::Impl::dequeueAll()
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
            std::queue<std::function<void()>> result = std::move(_jobs);
            _jobs = std::queue<std::function<void()>>(); // create new empty queue/
            return result;
        }

        void ThreadPool::Impl::wait()
        {
             std::unique_lock<std::mutex> lock(_stateMutex);
             return _totalReducedCountCv.wait(lock,[this]{
                 return _totalJobsCount==0 || !_running;
             });
        }

        bool ThreadPool::Impl::wait_for(const std::chrono::nanoseconds &timeout)
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            return _totalReducedCountCv.wait_for(lock,timeout,[this]{
                return _totalJobsCount==0 || !_running;
            });
        }

        unsigned int ThreadPool::Impl::threadCount() const
        {
            return _threadCount;
        }

        size_t ThreadPool::Impl::totalJobCount()
        {
             std::unique_lock<std::mutex> lock( _stateMutex);
             return _totalJobsCount;
        }

        size_t ThreadPool::Impl::queueCount()
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
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

        std::function<void()> ThreadPool::Impl::threadRetrieveCallback(const bool &reduceJobs)
        {
            std::function<void()> callback = nullptr;
            {
                std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
                if (reduceJobs) {
                    _totalJobsCount--;
                    _totalReducedCountCv.notify_all();
                }
                _jobsCv.wait(jobsMutexLock, [this]() { return !_jobs.empty() || !_running; });
                if (!_running) return nullptr;
                callback = std::move(_jobs.front());
                _jobs.pop();
            }
            return callback;
        }

        void ThreadPool::Impl::threadSpinner()
        {
            bool reduceJobs = false;
            while (_running) {
                std::function<void()> callback(threadRetrieveCallback(reduceJobs));
                reduceJobs = false;
                if (_running && callback != nullptr) {
                    callback();
                    reduceJobs = true;
                }
            }
        }

        void ThreadPool::Impl::lockedEnqueue(const std::vector<std::function<void()>> &jobs)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
            _totalJobsCount += jobs.size();
            for (const auto &job : jobs) {
                _jobs.push(job);
            }
        }


        void ThreadPool::Impl::lockedEnqueue(const std::function<void()> &job)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
            _totalJobsCount++;
            _jobs.push(job);
        }

        void ThreadPool::Impl::lockedEnqueue(std::vector<std::function<void()>> &&jobs)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
            _totalJobsCount += jobs.size();
            for (const auto &job : jobs) {
                _jobs.push(std::move(job));
            }
        }

        void ThreadPool::Impl::lockedEnqueue(std::queue<std::function<void()>> &&jobs)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
            _totalJobsCount += jobs.size();
            while (!jobs.empty()) {
                _jobs.push(std::move(jobs.front()));
                jobs.pop();
            }
        }

        void ThreadPool::Impl::lockedEnqueue(std::function<void()> &&job)
        {
            std::unique_lock<std::mutex> jobsMutexLock( _stateMutex );
            _totalJobsCount++;
            _jobs.push(std::move(job));
        }

        void ThreadPool::Impl::enqueue(const std::function<void()> &job)
        {
            lockedEnqueue(job);
            _jobsCv.notify_one(); // since one job is added, wake up one extra threads.
        }

        void ThreadPool::Impl::enqueue(const std::vector<std::function<void()>> &jobs)
        {
            lockedEnqueue(jobs);
            _jobsCv.notify_all(); // multiple jobs are added, wake up all threads.
        }

        void ThreadPool::Impl::enqueue(std::function<void()> &&job)
        {
            lockedEnqueue(std::move(job));
            _jobsCv.notify_one(); // since one job is added, wake up one extra threads.
        }

        void ThreadPool::Impl::enqueue(std::vector<std::function<void()>> &&jobs)
        {
            lockedEnqueue(std::move(jobs));
            _jobsCv.notify_all(); // multiple jobs are added, wake up all threads.
        }

        void ThreadPool::Impl::enqueue(std::queue<std::function<void()>> &&jobs)
        {
            lockedEnqueue(std::move(jobs));
            _jobsCv.notify_all(); // multiple jobs are added, wake up all threads.
        }

        ThreadPool::Impl::~Impl()
        {
            _running = false;
            { // scope to release the lock.
                std::unique_lock<std::mutex> jobsMutexLock( _stateMutex); // acquire lock, otherwise not all threads are waiting...
                _jobsCv.notify_all();
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

        size_t ThreadPool::totalJobCount()
        {
            return _impl->totalJobCount();
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

        void ThreadPool::wait()
        {
            _impl->wait();
        }

        bool ThreadPool::wait_for(const std::chrono::nanoseconds &timeout)
        {
             return _impl->wait_for(timeout);
        }

        std::function<void ()> ThreadPool::wrap(const std::function<void ()> &job)
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
