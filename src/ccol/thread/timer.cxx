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
#include <ccol/thread/timer.hxx>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <algorithm>

namespace ccol
{
    namespace thread
    {
        class Timer::Impl
        {
        private:
            std::mutex _stateLock;
            std::condition_variable _stateChanged;
            std::chrono::nanoseconds _interval;
            std::chrono::time_point<std::chrono::steady_clock> _nextInterval{std::chrono::steady_clock::now() + std::chrono::hours(24)};
            std::function<void()> _callBack;
            std::thread _thread;
            unsigned int _reliablity = 1;
            bool _running = false;
            bool _threadRunning = true;
            bool _changedState = false;
            void threadSpinner();
        public:
            Impl(const std::function<void (std::thread &)> &threadCreateCallback = nullptr);
            void start(const std::chrono::nanoseconds& delay, const std::chrono::nanoseconds& interval);
            void setReliability(const unsigned int &reliability);
            void setCallback(const std::function<void()> &callback);
            void setCallback(std::function<void()> &&callback);
            void stop();
            void threadStop();
            ~Impl();
        };

        Timer::Impl::Impl(const std::function<void (std::thread &)> &threadCreateCallback)
        {
            _thread = std::thread(&Timer::Impl::threadSpinner, this);
            if (threadCreateCallback!=nullptr) {
                threadCreateCallback(_thread);
            }
        }

        void Timer::Impl::threadSpinner()
        {
            std::function<void()> callBack;
            while (_threadRunning) {
                {
                    std::unique_lock<std::mutex> lock( _stateLock );
                    _changedState = false;
                    auto duration = _nextInterval - std::chrono::steady_clock::now() - std::chrono::milliseconds(_reliablity);
                    _stateChanged.wait_for(lock, duration, [this]() {
                        return !_threadRunning || _changedState;
                    });
                    if (_nextInterval > std::chrono::steady_clock::now()) { // extra check to improve accuracy.
                        continue;
                    }
                    if (!_running) {
                        _nextInterval = std::chrono::steady_clock::now() + std::chrono::hours(24);
                        continue;
                    }
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
                std::unique_lock<std::mutex> lock( _stateLock );
                _interval = interval;
                _nextInterval = std::chrono::steady_clock::now() + delay;
                if (!_running) {
                    _running = true;
                }
                _changedState = true;
            }
            _stateChanged.notify_all();
        }

        void Timer::Impl::setReliability(const unsigned int &reliability)
        {
            {
                std::unique_lock<std::mutex> lock( _stateLock );
                _reliablity = std::min(reliability,1000u);
                _changedState = true;
            }
            _stateChanged.notify_all();
        }

        void Timer::Impl::setCallback(const std::function<void()> &callback)
        {
            {
                std::unique_lock<std::mutex> lock( _stateLock );
                _callBack = callback;
                _changedState = true;
            }
            _stateChanged.notify_all();
        }

        void Timer::Impl::setCallback(std::function<void()> &&callback)
        {
            {
                std::unique_lock<std::mutex> lock( _stateLock );
                _callBack = std::move(callback);
                _changedState = true;
            }
            _stateChanged.notify_all();
        }

        void Timer::Impl::stop()
        {
            {
                std::unique_lock<std::mutex> lock( _stateLock );
                _running = false;
                _changedState = true;
            }
        }

        void Timer::Impl::threadStop()
        {
            {
                std::unique_lock<std::mutex> lock( _stateLock );
                _running = false;
                _threadRunning = false;
                _changedState = true;
                _stateChanged.notify_all();
            }
            if (_thread.joinable()) {
                _thread.join();
            }
        }

        Timer::Impl::~Impl()
        {
            threadStop();
        }

        Timer::Timer()
            : _impl(std::make_unique<Impl>())
        {
        }

        Timer::Timer(const std::function<void (std::thread &)> &threadCreateCallback)
            : _impl(std::make_unique<Impl>(threadCreateCallback))
        {

        }

        Timer::Timer(const std::function<void (std::thread &)> &&threadCreateCallback)
            : _impl(std::make_unique<Impl>(std::move(threadCreateCallback)))
        {

        }

        Timer::Timer(const std::function<void()> &callback)
            : _impl(std::make_unique<Impl>())
        {
            setCallback(callback);
        }

        Timer::Timer(std::function<void()> &&callback)
            : _impl(std::make_unique<Impl>())
        {
            setCallback(std::move(callback));
        }

        void Timer::setReliability(const unsigned int &reliability)
        {
            _impl->setReliability(reliability);
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
