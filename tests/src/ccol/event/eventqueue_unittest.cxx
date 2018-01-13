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
#include <ccol/event/callbackevent.hxx>
#include <ccol/event/dataevent.hxx>
#include "gtest/gtest.h"

TEST(EventQueue, EventQueueCallbackTest)
{
    ccol::event::EventQueue queue;
    bool callbackCalled = false;
    queue.setCallbackForType(typeid(ccol::event::CallbackEvent),[&queue](ccol::event::EventQueue::event_type &&event){
        auto callbackEvent = std::dynamic_pointer_cast<ccol::event::CallbackEvent>(event);
        if (callbackEvent!=nullptr) {
            callbackEvent->invoke();
        } else {
            queue.stop();
        }
    });
    auto event = std::make_shared<ccol::event::CallbackEvent>([&queue, &callbackCalled]{
        callbackCalled = true;
        queue.stop();
    });
    queue.enqueue(event);
    EXPECT_FALSE(callbackCalled);
    queue.run();
    EXPECT_TRUE(callbackCalled);
}

TEST(EventQueue, EventQueueDataCallbackTest)
{
    ccol::event::EventQueue queue;
    bool intCallbackCalled = false;
    bool longCallbackCalled = false;
    bool callbackCalled = false;

    queue.setCallbackForType(typeid(ccol::event::CallbackEvent),[&queue](ccol::event::EventQueue::event_type &&event){
        auto callbackEvent = std::dynamic_pointer_cast<ccol::event::CallbackEvent>(event);
        if (callbackEvent!=nullptr) {
            callbackEvent->invoke();
        } else {
            queue.stop();
        }
    });

    typedef ccol::event::StaticDataEvent<int> IntEvent;
    queue.setCallbackForType(typeid(IntEvent),[&queue,&intCallbackCalled](ccol::event::EventQueue::event_type &&event){
        auto dataEvent = std::dynamic_pointer_cast<IntEvent>(event);
        if (dataEvent!=nullptr) {
            EXPECT_EQ(-10,dataEvent->dataRef());
            EXPECT_EQ(-10,dataEvent->dataCopy());
            intCallbackCalled = true;
        } else {
            queue.stop();
        }
    });

    typedef ccol::event::SharedDataEvent<long> SharedLongEvent;
    queue.setCallbackForType(typeid(SharedLongEvent),[&queue,&longCallbackCalled](ccol::event::EventQueue::event_type &&event){

        auto dataEvent = std::dynamic_pointer_cast<SharedLongEvent>(event);
        if (dataEvent!=nullptr) {
            EXPECT_EQ(18,*dataEvent->dataRef());
            EXPECT_EQ(18,*dataEvent->dataCopy());
            longCallbackCalled = true;
        } else {
            queue.stop();
        }
    });

    queue.enqueue(std::make_shared<IntEvent>(-10));
    queue.enqueue(std::make_shared<SharedLongEvent>(std::make_shared<long>(18)));
    queue.enqueue(std::make_shared<ccol::event::CallbackEvent>([&queue, &callbackCalled]{ callbackCalled = true; queue.stop(); }));
    EXPECT_FALSE(intCallbackCalled);
    EXPECT_FALSE(longCallbackCalled);
    EXPECT_FALSE(callbackCalled);
    queue.run();
    EXPECT_TRUE(intCallbackCalled);
    EXPECT_TRUE(intCallbackCalled);
    EXPECT_TRUE(callbackCalled);
}

TEST(EventQueue, QueueLimitTest)
{
    FAIL();
}

TEST(EventQueue, QueueLimitTest)
{
    FAIL();
}


