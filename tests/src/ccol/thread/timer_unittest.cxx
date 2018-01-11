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
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <queue>
#include <thread>
#include "gtest/gtest.h"

using namespace std::literals::chrono_literals;

const auto interval = 200ms;
const unsigned int reliablity = 50;

struct TimerTestContext
{
    std::mutex mutex;
    std::atomic_int count{0};
    std::chrono::steady_clock::time_point start{std::chrono::steady_clock::now()};
    std::chrono::steady_clock::time_point previous{std::chrono::steady_clock::now()};
    std::function<void()> onTimerAdd{[this]{
            count.fetch_add(1);
    }};
    std::function<void()> onTimerSub{[this]{
            count.fetch_sub(1);
    }};
};

TEST(Timer, Singleshot)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.startSingleshot(interval);
    std::this_thread::sleep_for(interval * 3);

    EXPECT_EQ(1,t.count.load());
}

TEST(Timer, Interval)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(interval);
    std::this_thread::sleep_for((interval*4));
    std::this_thread::sleep_for((interval/2));

    EXPECT_EQ(4,t.count.load());
}

TEST(Timer, NoDelayInterval)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(0ms,interval);
    std::this_thread::sleep_for((interval*4));
    std::this_thread::sleep_for((interval/2));

    EXPECT_EQ(5,t.count.load());
}

TEST(Timer, LongDelayInterval)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(interval*2,interval);
    std::this_thread::sleep_for((interval*4));
    std::this_thread::sleep_for((interval/2));

    EXPECT_EQ(3,t.count.load());
}

TEST(Timer, IntervalReschedule)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(interval);
    std::this_thread::sleep_for((interval*2));
    std::this_thread::sleep_for((interval/2));
    timer.start(interval*2);
    std::this_thread::sleep_for((interval*2));

    EXPECT_EQ(3,t.count.load());
}


TEST(Timer, IntervalToSingleshot)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(interval);
    std::this_thread::sleep_for((interval*2));
    std::this_thread::sleep_for((interval/2));
    timer.startSingleshot(interval);
    std::this_thread::sleep_for((interval*4));

    EXPECT_EQ(3,t.count.load());
}

TEST(Timer, IntervalReplaceCallback)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(interval);
    std::this_thread::sleep_for((interval*2));
    std::this_thread::sleep_for((interval/2));
    timer.setCallback(t.onTimerSub);
    EXPECT_EQ(2,t.count.load());
    std::this_thread::sleep_for((interval*2));

    EXPECT_EQ(0,t.count.load());
}

TEST(Timer, SingleshotSetupCallbackViaConstructor)
{
    TimerTestContext t;
    ccol::thread::Timer timer(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.startSingleshot(interval);
    std::this_thread::sleep_for(interval * 3);

    EXPECT_EQ(1,t.count.load());
}


TEST(Timer, IntervalStop)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(interval);
    std::this_thread::sleep_for(interval*2);
    std::this_thread::sleep_for(interval/2);
    timer.stop();
    EXPECT_EQ(2,t.count.load());
    std::this_thread::sleep_for(interval*2);


    EXPECT_EQ(2,t.count.load());
}



TEST(Timer, IntervalStopRestart)
{
    TimerTestContext t;
    ccol::thread::Timer timer;
    timer.setCallback(t.onTimerAdd);
    timer.setReliability(reliablity);

    timer.start(interval);
    std::this_thread::sleep_for(interval*2);
    std::this_thread::sleep_for(interval/2);
    timer.stop();
    EXPECT_EQ(2,t.count.load());
    std::this_thread::sleep_for(interval*2);
    EXPECT_EQ(2,t.count.load());
    timer.start(interval);
    std::this_thread::sleep_for(interval*2);
    std::this_thread::sleep_for(interval/2);

    EXPECT_EQ(4,t.count.load());
}


