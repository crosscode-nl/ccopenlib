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
#include <ccol/thread/threadpool.hxx>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <queue>
#include "gtest/gtest.h"

namespace {

TEST(ThreadPool, InstantiatedThreadsCorrect)
{
    ccol::thread::ThreadPool threadpool;
    EXPECT_EQ(std::thread::hardware_concurrency(),threadpool.threadCount());
}

TEST(ThreadPool, InstantiatedSpecified2ThreadsCorrect)
{
    ccol::thread::ThreadPool threadpool(2);
    EXPECT_EQ(2,threadpool.threadCount());
}

TEST(ThreadPool, InstantiatedSpecified10ThreadsCorrect)
{
    ccol::thread::ThreadPool threadpool(10);
    EXPECT_EQ(10,threadpool.threadCount());
}

TEST(ThreadPool, InstantiatedSpecified5ThreadCallBackCalled5Times)
{
    int count = 0;
    ccol::thread::ThreadPool threadpool(5,[&count](std::thread&){
        count++;
    });
    EXPECT_EQ(5,count);
}

struct ThreadPoolTestContext
{
    std::condition_variable cv;
    std::condition_variable cv2;
    std::mutex synchronizerMutex;
    std::mutex waitTerminateMutex;
    std::atomic_int endCount = {0};
    std::atomic_int middleCount = {0};
    std::unique_lock<std::mutex> syncLock{synchronizerMutex};
    std::function<void(void)> job = [this]{
        {
            std::unique_lock<std::mutex> syncLock2(synchronizerMutex);
        }
        middleCount++;
        cv.notify_one();
        {
            std::unique_lock<std::mutex> terminateLock(waitTerminateMutex);
        }
        endCount++;
        cv2.notify_one();
    };
};

TEST(ThreadPool, ExecutedTwoThreads)
{
    using namespace std::literals::chrono_literals;

    ThreadPoolTestContext tptc;
    EXPECT_TRUE(tptc.syncLock.owns_lock());
    ccol::thread::ThreadPool threadpool(2);

    {
        std::unique_lock<std::mutex> lk2(tptc.waitTerminateMutex);
        threadpool.enqueue(tptc.job);
        threadpool.enqueue(tptc.job);

        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==2; }));
        EXPECT_EQ(0,threadpool.queueCount());
    }
    EXPECT_EQ(0,threadpool.queueCount());
    EXPECT_EQ(2,threadpool.threadCount());
    EXPECT_EQ(2,tptc.middleCount);
}

TEST(ThreadPool, ExecutedTwoOfFourThreads)
{
    using namespace std::literals::chrono_literals;

    ThreadPoolTestContext tptc;
    ccol::thread::ThreadPool threadpool(2);
    {
        std::unique_lock<std::mutex> lk2(tptc.waitTerminateMutex);

        threadpool.enqueue(tptc.job);
        threadpool.enqueue(tptc.job);
        threadpool.enqueue(tptc.job);
        threadpool.enqueue(tptc.job);

        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==2; }));
        EXPECT_EQ(2,threadpool.queueCount());
        EXPECT_EQ(2,threadpool.dequeueAll().size());
        EXPECT_EQ(0,threadpool.queueCount());
    }
    EXPECT_EQ(2,threadpool.threadCount());
    EXPECT_EQ(2,tptc.middleCount);
}

TEST(ThreadPool, ExecutedFourThreadsFromVector)
{
    using namespace std::literals::chrono_literals;

    ThreadPoolTestContext tptc;
    ccol::thread::ThreadPool threadpool(2);
    {
        std::unique_lock<std::mutex> lk2(tptc.waitTerminateMutex);

        threadpool.enqueue(std::vector<std::function<void()>>{tptc.job,tptc.job,tptc.job,tptc.job});

        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==2; }));
        EXPECT_TRUE(tptc.cv2.wait_for(lk2, 200ms, [&tptc]{ return tptc.endCount==2; }));
        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==4; }));
        EXPECT_TRUE(tptc.cv2.wait_for(lk2, 200ms, [&tptc]{ return tptc.endCount==4; }));
        EXPECT_EQ(0,threadpool.queueCount());
    }
    EXPECT_EQ(0,threadpool.queueCount());
    EXPECT_EQ(2,threadpool.threadCount());
    EXPECT_EQ(4,tptc.middleCount);
}

TEST(ThreadPool, ExecutedFourThreadsFromQueue)
{
    using namespace std::literals::chrono_literals;

    ThreadPoolTestContext tptc;
    ccol::thread::ThreadPool threadpool(2);
    {
        std::unique_lock<std::mutex> lk2(tptc.waitTerminateMutex);

        std::queue<std::function<void()>> jobs;
        for (int counter=0; counter<4; counter++) {
            jobs.push(tptc.job);
        }

        threadpool.enqueue(jobs);

        EXPECT_EQ(4,jobs.size());
        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==2; }));
        EXPECT_TRUE(tptc.cv2.wait_for(lk2, 200ms, [&tptc]{ return tptc.endCount==2; }));
        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==4; }));
        EXPECT_TRUE(tptc.cv2.wait_for(lk2, 200ms, [&tptc]{ return tptc.endCount==4; }));
        EXPECT_EQ(0,threadpool.queueCount());
    }
    EXPECT_EQ(0,threadpool.queueCount());
    EXPECT_EQ(2,threadpool.threadCount());
    EXPECT_EQ(4,tptc.middleCount);
}

TEST(ThreadPool, ExecutedFourOfFourThreadsWithDequeInBetween)
{
    using namespace std::literals::chrono_literals;

    ThreadPoolTestContext tptc;
    ccol::thread::ThreadPool threadpool(2);
    std::queue<std::function<void()>> jobs;
    {
        std::unique_lock<std::mutex> lk2(tptc.waitTerminateMutex);
        threadpool.enqueue(tptc.job);
        threadpool.enqueue(tptc.job);
        threadpool.enqueue(tptc.job);
        threadpool.enqueue(tptc.job);
        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==2; }));
        EXPECT_EQ(2,threadpool.queueCount());
        EXPECT_EQ(2,tptc.middleCount);
        jobs = threadpool.dequeueAll();
        EXPECT_EQ(0,threadpool.queueCount());
        EXPECT_TRUE(tptc.cv2.wait_for(lk2, 200ms, [&tptc]{ return tptc.endCount==2; }));
        EXPECT_EQ(2,tptc.endCount);
        threadpool.enqueue(jobs);
        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==4; }));
        EXPECT_TRUE(tptc.cv2.wait_for(lk2, 200ms, [&tptc]{ return tptc.endCount==4; }));
    }

    EXPECT_EQ(2,threadpool.threadCount());
    EXPECT_EQ(4,tptc.middleCount);
}

TEST(ThreadPool, ExecutedTwoOfFourEmittedByWrapper)
{
    using namespace std::literals::chrono_literals;

    ThreadPoolTestContext tptc;
    ccol::thread::ThreadPool threadpool(2);
    {
        std::unique_lock<std::mutex> lk2(tptc.waitTerminateMutex);

        auto wrappedJob = threadpool.wrap(tptc.job);
        wrappedJob();
        wrappedJob();
        wrappedJob();
        wrappedJob();

        EXPECT_TRUE(tptc.cv.wait_for(tptc.syncLock, 200ms, [&tptc]{ return tptc.middleCount==2; }));
        EXPECT_EQ(2,threadpool.queueCount());
        threadpool.clear();
        EXPECT_EQ(0,threadpool.queueCount());
    }
    EXPECT_EQ(2,threadpool.threadCount());
    EXPECT_EQ(2,tptc.middleCount);
}


}
