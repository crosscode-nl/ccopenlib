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
#include <ccol/event/callbackeventqueue.hxx>
#include <ccol/event/callbackevent.hxx>

namespace ccol {

    namespace event {

        void callback(std::shared_ptr<BaseEvent> &&event)
        {
            auto callbackEvent = std::dynamic_pointer_cast<ccol::event::CallbackEvent>(event);
            if (callbackEvent!=nullptr) {
                callbackEvent->invoke();
            }
        }

        CallbackEventQueue::CallbackEventQueue()
        {
            _eventQueue.setCallbackForType(typeid(CallbackEvent),&callback);
        }

        CallbackEventQueue::CallbackEventQueue(const std::size_t &maxQueueSize)
            : _eventQueue(maxQueueSize)
        {
            _eventQueue.setCallbackForType(typeid(CallbackEvent),&callback);
        }

        bool CallbackEventQueue::enqueue(const std::function<void ()> &function)
        {
            return _eventQueue.enqueue(std::make_shared<CallbackEvent>(function));
        }

        bool CallbackEventQueue::enqueue(std::function<void ()> &&function)
        {
            return _eventQueue.enqueue(std::make_shared<CallbackEvent>(std::move(function)));
        }

        void CallbackEventQueue::run()
        {
            _eventQueue.run();
        }

        bool CallbackEventQueue::isRunning()
        {
            return _eventQueue.isRunning();
        }

        void CallbackEventQueue::stop()
        {
            _eventQueue.stop();
        }

        std::function<bool()> CallbackEventQueue::wrap(const std::function<void()> &function)
        {
            return [function,this]{
                return _eventQueue.enqueue(std::make_shared<CallbackEvent>(function));
            };
        }

        CallbackEventQueue::~CallbackEventQueue()
        {
            _eventQueue.stop();
        }

    }

}
