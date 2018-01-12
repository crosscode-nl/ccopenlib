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
#include <ccol/thread/thread_wrap.hxx>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <queue>
#include <thread>
#include "gtest/gtest.h"



TEST(thread_wrap, ExecutesInThread)
{
    std::mutex _stateMutex;
    std::condition_variable _stateCv;
    std::atomic_bool v(false);
    std::thread::id currentId = std::this_thread::get_id();
    std::thread::id threadId = std::this_thread::get_id();
    std::unique_lock<std::mutex> mainLock(_stateMutex);
    auto wrapper = ccol::thread::thread_wrap([&v, &threadId, &_stateMutex, &_stateCv]{
       {
            std::unique_lock<std::mutex> lock(_stateMutex);
            v.store(true);
            threadId = std::this_thread::get_id();
       }
       _stateCv.notify_one();
    });
    EXPECT_FALSE(v.load());
    EXPECT_EQ(currentId,threadId);
    wrapper();
    _stateCv.wait(mainLock);
    EXPECT_TRUE(v.load());
    EXPECT_NE(currentId,threadId);
}
