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
#ifndef CCOL_EVENT_EVENTQUEUE_HXX
#define CCOL_EVENT_EVENTQUEUE_HXX
#include <ccol/event/baseevent.hxx>
#include <functional>
#include <utility>
#include <vector>
#include <memory>
#include <typeinfo>
#include <typeindex>


namespace ccol {
    namespace event {

        class EventQueue
        {
            private:
            class Impl;
            std::unique_ptr<Impl> _impl;
            public:
            typedef std::shared_ptr<BaseEvent> event_type;
            typedef std::function<void(event_type&&)> callback_type;
            typedef std::vector<std::pair<std::type_index,callback_type>> callback_vector_type;
            EventQueue();
            EventQueue(const std::size_t &maxQueueSize);
            bool enqueue(const event_type &event);
            bool enqueue(event_type &&event);
            void setCallbackForType(const std::type_index &type, const callback_type &callback);
            void setCallbackForType(const std::type_index &type, callback_type &&callback);
            void setCallbacks(const callback_vector_type &callbacks);
            void setCallbacks(callback_vector_type &&callbacks);
            void run();
            bool isRunning();
            void stop();
            virtual ~EventQueue();
        };

    }
}
#endif
