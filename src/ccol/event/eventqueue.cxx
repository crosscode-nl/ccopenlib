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
#include <ccol/event/eventqueue.hxx>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <queue>
#include <typeindex>

namespace ccol {

    namespace event {

        class EventQueue::Impl
        {
            private:
            std::mutex _stateMutex;
            std::condition_variable _stateCv;
            std::size_t _maxQueueSize;
            std::unordered_map<std::type_index,callback_type> _callbacks;
            std::queue<event_type> _events;
            bool _running = false;
            public:
            Impl(const std::size_t &maxQueueSize);
            bool enqueue(const event_type &event);
            bool enqueue(event_type &&event);
            void setCallbackForType(const std::type_index &type, const callback_type &callback);
            void setCallbackForType(const std::type_index &type, callback_type &&callback);
            void setCallbacks(const callback_vector_type &callbacks);
            void setCallbacks(callback_vector_type &&callbacks);
            void run();
            bool isRunning();
            void stop();
        };

        EventQueue::Impl::Impl(const std::size_t &maxQueueSize)
            : _maxQueueSize(maxQueueSize)
        {

        }

        bool EventQueue::Impl::enqueue(const event_type &event)
        {
            {
                std::unique_lock<std::mutex> lock(_stateMutex);
                if (_maxQueueSize>0 && _events.size()>=_maxQueueSize) return false;
                _events.push(event);
            }
            _stateCv.notify_all();
            return true;
        }

        bool EventQueue::Impl::enqueue(event_type &&event)
        {
            {
                std::unique_lock<std::mutex> lock(_stateMutex);
                if (_maxQueueSize>0 && _events.size()>=_maxQueueSize) return false;
                _events.push(std::move(event));
            }
            _stateCv.notify_all();
            return true;
        }

        void EventQueue::Impl::setCallbackForType(const std::type_index &type, const callback_type &callback)
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            _callbacks[type] = callback;
        }

        void EventQueue::Impl::setCallbackForType(const std::type_index &type, callback_type &&callback)
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            _callbacks[type] = std::move(callback);
        }

        void EventQueue::Impl::setCallbacks(const callback_vector_type &callbacks)
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            for (const auto &typeAndCallback : callbacks) {
                _callbacks[typeAndCallback.first] = typeAndCallback.second;
            }
        }

        void EventQueue::Impl::setCallbacks(callback_vector_type &&callbacks)
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            for (const auto &typeAndCallback : callbacks) {
                _callbacks[typeAndCallback.first] = std::move(typeAndCallback.second);
            }
        }

        void EventQueue::Impl::run()
        {
             if (_running) return;
             _running = true;
             while (_running) {
                 callback_type callback = nullptr;
                 event_type event;
                 {
                     std::unique_lock<std::mutex> lock(_stateMutex);
                     _stateCv.wait(lock,[this]{
                        return _events.size()>0 || !_running;
                     });
                     if (!_running) break;
                     if (_events.size()>0) {
                        event = _events.front();
                        _events.pop();
                        auto callbackIterator = _callbacks.find(typeid(*event));
                        if (callbackIterator!=_callbacks.end()) {
                            callback = callbackIterator->second;
                        }
                     }
                 }
                 if (callback!=nullptr) {
                     callback(std::move(event));
                 }
             }
        }

        bool EventQueue::Impl::isRunning()
        {
            std::unique_lock<std::mutex> lock(_stateMutex);
            return _running;
        }

        void EventQueue::Impl::stop()
        {
            {
                std::unique_lock<std::mutex> lock(_stateMutex);
                _running = false;
            }
            _stateCv.notify_all();
        }

        EventQueue::EventQueue()
            : _impl(std::make_unique<Impl>(0))
        {
        }

        EventQueue::EventQueue(const std::size_t &maxQueueSize)
            : _impl(std::make_unique<Impl>(maxQueueSize))
        {
        }

        bool EventQueue::enqueue(const event_type &event)
        {
            return _impl->enqueue(event);
        }

        bool EventQueue::enqueue(event_type &&event)
        {
            return _impl->enqueue(std::move(event));
        }

        void EventQueue::setCallbackForType(const std::type_index &type, const callback_type &callback)
        {
            _impl->setCallbackForType(type,callback);
        }

        void EventQueue::setCallbackForType(const std::type_index &type, callback_type &&callback)
        {
            _impl->setCallbackForType(type,std::move(callback));
        }

        void EventQueue::setCallbacks(const callback_vector_type &callbacks)
        {
            _impl->setCallbacks(callbacks);
        }

        void EventQueue::setCallbacks(callback_vector_type &&callbacks)
        {
            _impl->setCallbacks(std::move(callbacks));
        }

        bool EventQueue::isRunning()
        {
            return _impl->isRunning();
        }

        void EventQueue::run()
        {
            _impl->run();
        }

        void EventQueue::stop()
        {
            _impl->stop();
        }

        EventQueue::~EventQueue()
        {
            stop();
        }

    }

}
