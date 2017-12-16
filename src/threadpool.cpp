/**
		© 2017 CrossCode / Patrick Vollebregt - All rights reserved

		SPDX-License-Identifier: CC-BY-SA-4.0

		This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
		To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA

		If you use this code, please mention the name CrossCode in your product.
		If for some reason this not possible, please contact: ccopenlib@crosscode.nl to obtain a license.

		If you'd like to modify and/or share this code, please share it under the same license, and keep the copyright notice intact.

		If you found any errors or improvements you'd like to share? Please contact me: ccopenlib@crosscode.nl

		Visit http://www.ccopenlib.com for more information.
*/
#include <threadpool.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <iostream>

namespace ccol
{	
		class ThreadPool::Impl
		{
		private:
			volatile bool _running = true;
			unsigned int _threadCount = 0;
			std::vector<std::thread> _threads;
			std::condition_variable _threadUnlocked;
			std::queue<std::function<void()>> _jobs;
			std::mutex _jobsMutex;
			void threadSpinner();
			inline std::function<void()> threadRetrieveCallback();
			inline void lockedEnqueueJob(const std::vector<std::function<void()>> &methods);			
			inline void lockedEnqueueJob(const std::function<void()> &method);
			inline void lockedEnqueueJob(std::vector<std::function<void()>> &&methods);
			inline void lockedEnqueueJob(std::queue<std::function<void()>> &&methods);
			inline void lockedEnqueueJob(std::function<void()> &&method);
		public:
			Impl(const unsigned int &threads);
			inline void enqueueJob(const std::function<void()> &method);
			inline void enqueueJob(const std::vector<std::function<void()>> &methods);			
			inline void enqueueJob(std::function<void()> &&method);
			inline void enqueueJob(std::vector<std::function<void()>> &&methods);
			inline void enqueueJob(std::queue<std::function<void()>> &&methods);
			inline size_t jobsInQueueCount();
			inline unsigned int threadCount();
			inline void clearQueue();
			inline std::queue<std::function<void()>> pullJobsFromQueue();
			~Impl();
		};

		inline void ThreadPool::Impl::clearQueue()
		{

		}

		inline std::queue<std::function<void()>> ThreadPool::Impl::pullJobsFromQueue()
		{
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			std::queue<std::function<void()>> result = std::move(_jobs);
			_jobs = std::queue<std::function<void()>>(); // create new empty queue/
			return std::move(result);
		}

		unsigned int ThreadPool::Impl::threadCount()
		{
			return _threadCount;
		}

		size_t ThreadPool::Impl::jobsInQueueCount()
		{
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			return _jobs.size();
		}

		ThreadPool::Impl::Impl(const unsigned int &threads)
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
				_threads.push_back(std::thread(&Impl::threadSpinner, this));
			}
		}

		std::function<void()> ThreadPool::Impl::threadRetrieveCallback()
		{
			std::function<void()> callback = nullptr;
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			_threadUnlocked.wait(jobsMutexLock, [this]() { return !_jobs.empty() || !_running; });
			if (!_running) return nullptr;
			callback = std::move(_jobs.front());
			_jobs.pop();
			return std::move(callback);
		}

		void ThreadPool::Impl::threadSpinner()
		{
			try {
				while (_running) {
					std::function<void()> callback(threadRetrieveCallback());
					if (_running && callback != nullptr) {
						callback();
					}
				}
			}
			catch (std::exception &e) {
				std::cout << e.what();
				throw;
			}
		}

		void ThreadPool::Impl::lockedEnqueueJob(const std::vector<std::function<void()>> &methods)
		{
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			for (const auto &method : methods) {
				_jobs.push(method);
			}
		}
	

		void ThreadPool::Impl::lockedEnqueueJob(const std::function<void()> &method)
		{
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			_jobs.push(method);
		}

		void ThreadPool::Impl::lockedEnqueueJob(std::vector<std::function<void()>> &&methods)
		{
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			for (const auto &method : methods) {
				_jobs.push(std::move(method));
			}
		}

		void ThreadPool::Impl::lockedEnqueueJob(std::queue<std::function<void()>> &&methods)
		{
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			while (!methods.empty()) {
				_jobs.push(std::move(methods.front()));
				methods.pop();
			}
		}

		void ThreadPool::Impl::lockedEnqueueJob(std::function<void()> &&method)
		{
			std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex);
			_jobs.push(std::move(method));
		}

		void ThreadPool::Impl::enqueueJob(const std::function<void()> &method)
		{
			lockedEnqueueJob(method);
			_threadUnlocked.notify_one(); // since one job is added, wake up one extra threads. 
		}

		void ThreadPool::Impl::enqueueJob(const std::vector<std::function<void()>> &methods)
		{
			lockedEnqueueJob(methods);
			_threadUnlocked.notify_all(); // multiple jobs are added, wake up all threads. 
		}


		void ThreadPool::Impl::enqueueJob(std::function<void()> &&method)
		{
			lockedEnqueueJob(std::move(method));
			_threadUnlocked.notify_one(); // since one job is added, wake up one extra threads. 
		}

		void ThreadPool::Impl::enqueueJob(std::vector<std::function<void()>> &&methods)
		{
			lockedEnqueueJob(std::move(methods));
			_threadUnlocked.notify_all(); // multiple jobs are added, wake up all threads. 
		}

		void ThreadPool::Impl::enqueueJob(std::queue<std::function<void()>> &&methods)
		{
			lockedEnqueueJob(std::move(methods));
			_threadUnlocked.notify_all(); // multiple jobs are added, wake up all threads. 
		}

		ThreadPool::Impl::~Impl()
		{
			_running = false;
			{ // scope to release the lock.
				std::unique_lock<std::mutex> jobsMutexLock(_jobsMutex); // acquire lock, otherwise not all threads are waiting... 
				_threadUnlocked.notify_all();
			}

			for (std::thread &thread : _threads) {
				thread.join();
			}
		}

		ThreadPool::ThreadPool()
			: _impl(std::make_unique<Impl>(0))
		{

		}

		ThreadPool::ThreadPool(const unsigned int &threads)
			: _impl(std::make_unique<Impl>(threads))
		{

		}

		void ThreadPool::enqueueJob(const std::function<void()> &method)
		{
			_impl->enqueueJob(method);
		}

		void ThreadPool::enqueueJob(const std::vector<std::function<void()>> &methods)
		{
			_impl->enqueueJob(methods);
		}

		void ThreadPool::enqueueJob(std::queue<std::function<void()>> methods)
		{
			_impl->enqueueJob(std::move(methods));
		}

		void ThreadPool::enqueueJob(std::function<void()> &&method)
		{
			_impl->enqueueJob(std::move(method));
		}

		void ThreadPool::enqueueJob(std::vector<std::function<void()>> &&methods)
		{
			_impl->enqueueJob(std::move(methods));
		}

		void ThreadPool::enqueueJob(std::queue<std::function<void()>> &&methods)
		{
			_impl->enqueueJob(std::move(methods));
		}

		size_t ThreadPool::jobsInQueueCount()
		{
			return _impl->jobsInQueueCount();
		}

		unsigned int ThreadPool::threadCount()
		{
			return _impl->threadCount();
		}

		void ThreadPool::clearQueue()
		{
			return _impl->clearQueue();
		}

		std::queue<std::function<void()>> ThreadPool::pullJobsFromQueue()
		{
			return _impl->pullJobsFromQueue();
		}

		ThreadPool::~ThreadPool()
		{

		}	
}