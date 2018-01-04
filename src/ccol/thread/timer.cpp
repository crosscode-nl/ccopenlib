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
#include <ccol/thread/timer.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <chrono>
namespace ccol
{
	namespace thread
	{
		class Timer::Impl
		{
		private:
			std::mutex _stateLock;
			std::condition_variable _stateChanged;
			std::chrono::nanoseconds _delay;
			std::chrono::nanoseconds _interval;
			std::chrono::time_point<std::chrono::steady_clock> _nextInterval;
			bool _running = false;
			std::function<void()> _callBack;
			std::thread _thread;
			void threadSpinner();
		public:
			Impl();
			void start(const std::chrono::nanoseconds& delay, const std::chrono::nanoseconds& interval);
			void setCallback(const std::function<void()> &callback);
			void setCallback(std::function<void()> &&callback);
			void stop();
			~Impl();
		};

		Timer::Impl::Impl()
		{

		}


		void Timer::Impl::threadSpinner()
		{
			std::function<void()> callBack;
			while (_running) {
				{
					std::unique_lock<std::mutex> lock{ _stateLock };
					_stateChanged.wait_until(lock, _nextInterval, [this]() {
						return !_running || _nextInterval <= std::chrono::steady_clock::now();
					});
					if (!_running) break;
					callBack = _callBack; // make copy of callback, so it can execute outside a lock and meanwhile be changed.
					if (_interval > std::chrono::nanoseconds(0)) {
						_nextInterval = std::chrono::steady_clock::now() + _interval;
					}
					else {
						_running = false; // stop after executing callback.
					}
				}
				if (callBack != nullptr) {
					callBack();
				}
			}
		}


		void Timer::Impl::start(const std::chrono::nanoseconds & delay, const std::chrono::nanoseconds& interval)
		{
			{
				std::unique_lock<std::mutex> lock{ _stateLock };
				_delay = delay;
				_interval = interval;
				_nextInterval = std::chrono::steady_clock::now() + delay;
				if (!_running) {
					_running = true;
					_thread = std::thread(&Timer::Impl::threadSpinner, this);
				}
			}
			_stateChanged.notify_all();
		}

		void Timer::Impl::setCallback(const std::function<void()> &callback)
		{
			{
				std::unique_lock<std::mutex> lock{ _stateLock };
				_callBack = callback;
			}
			_stateChanged.notify_all();
		}

		void Timer::Impl::setCallback(std::function<void()> &&callback)
		{
			{
				std::unique_lock<std::mutex> lock{ _stateLock };
				_callBack = std::move(callback);
			}
			_stateChanged.notify_all();
		}

		void Timer::Impl::stop()
		{
			std::unique_lock<std::mutex> lock{ _stateLock };
			_running = false;
			_stateChanged.notify_all();
			if (_thread.joinable()) {
				_thread.join();
			}
		}

		Timer::Impl::~Impl()
		{
			stop();
		}

		Timer::Timer()
			: _impl(std::make_unique<Impl>())
		{

		}

		void Timer::start(const std::chrono::nanoseconds& delay, const std::chrono::nanoseconds& interval)
		{
			_impl->start(delay, interval);
		}

		void Timer::start(const std::chrono::nanoseconds& interval)
		{
			_impl->start(interval, interval);
		}

		void Timer::startSingleshot(const std::chrono::nanoseconds& delay)
		{
			_impl->start(delay, std::chrono::microseconds(0));
		}

		void Timer::setCallback(const std::function<void()> &callback)
		{
			_impl->setCallback(callback);
		}

		void Timer::setCallback(std::function<void()> &&callback)
		{
			_impl->setCallback(std::move(callback));
		}

		void Timer::stop()
		{
			_impl->stop();
		}

		Timer::~Timer()
		{

		}
	}
}