/**
		� 2017 CrossCode / Patrick Vollebregt - All rights reserved

		SPDX-License-Identifier: CC-BY-SA-4.0

		This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
		To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA

		If you use this code, please mention the name CrossCode in your product. 
		If for some reason this not possible, please contact: ccopenlib@crosscode.nl to obtain a license.

		If you'd like to modify and/or share this code, please share it under the same license, and keep the copyright notice intact.

		If you found any errors or improvements you'd like to share? Please contact me: ccopenlib@crosscode.nl

		Visit http://www.ccopenlib.com for more information.
*/
#ifndef CCOPENLIB_THREADPOOL_H
#define CCOPENLIB_THREADPOOL_H

#include <memory>
#include <vector>
#include <queue>
#include <functional>

namespace ccol
{
	class ThreadPool
	{
	private:
		class Impl;
		std::unique_ptr<Impl> _impl;
	public:
		ThreadPool();
		ThreadPool(const unsigned int &threads);
		void enqueueJob(const std::function<void()> &method);
		void enqueueJob(const std::vector<std::function<void()>> &methods);
		void enqueueJob(std::queue<std::function<void()>> methods); // copy because we need to pop...
		void enqueueJob(std::function<void()> &&method);
		void enqueueJob(std::vector<std::function<void()>> &&methods);
		void enqueueJob(std::queue<std::function<void()>> &&methods);
		size_t jobsInQueueCount();
		unsigned int threadCount();
		void clearQueue();
		std::queue<std::function<void()>> pullJobsFromQueue();
		virtual ~ThreadPool();
	};
}

#endif // CCOPENLIB_THREADPOOL_H