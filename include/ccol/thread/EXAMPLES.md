# Thread Examples

## ThreadPool

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/thread/threadpool.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To create a ThreadPool with an optimal amount of Threads:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::ThreadPool threadpool;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To create a ThreadPool with a specific amount of Threads:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::ThreadPool threadpool(2); // create 2 threads on the ThreadPool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To execute on the ThreadPool:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::ThreadPool threadpool;
threadpool.enqueue([]{
        // some code that will run on the ThreadPool
});
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To create a wrapper around a lambda that will run on the ThreadPool each time it is invoked.
You could use this in combination with a Timer to run Timer events on the ThreadPool, for example.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::ThreadPool threadpool;
auto wrappedlambda = threadpool.wrap([]{
        // some code that will run on the ThreadPool LATER
});
for (int counter=0; counter<10; counter++) { // will create 10 jobs on the ThreadPool
        wrappedlambda(); // will run on the ThreadPool
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The amount of jobs waiting to be processed can be retrieved by calling:

~~~~~~~~~~~~~~~~~~~~~~~~cpp
threadpool.queueCount();
~~~~~~~~~~~~~~~~~~~~~~~~

The amount of thread available can be retrieved with the following call:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::ThreadPool threadpool(2); // create 2 threads on the ThreadPool
threadpool.threadCount(); // returns 2.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To pause or stop processing you can pull queued jobs from the threadpool.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
auto jobs = threadpool.dequeueAll();
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

And to resume the jobs pulled from the threadpool you can enqueue them again.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
auto jobs = threadpool.dequeueAll();
threadpool.enqueue(jobs);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

See tests for more complete working examples.

## Timer

The following examples require the following include headers and using namespace statement.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/thread/timer.hxx>
#include <chrono>

using namespace std::literals::chrono_literals;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create a timer that fires once.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::Timer timer([]{
    // do some work when the timer fires
});

timer.startSingleshot(5s); // fire after 5 seconds
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create a timer that fires every 5 seconds.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::Timer timer([]{
    // do some work when the timer fires
});

timer.start(5s); // fire after 5 seconds
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create a timer that fires immediately and then every 500 milliseconds.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::Timer timer([]{
    // do some work when the timer fires
});

timer.start(0ms, 500ms); // fire after 500 milliseconds
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Stop a timer.

A timer can be stopped and restarted by using start or startSingleshot everytime. Stop is not required betweeen those calls.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::Timer timer([]{
    // do some work when the timer fires
});

timer.start(0ms, 500ms); // fire after 500 milliseconds
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Change or set a callback. The callback can be changed at any time.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::thread::Timer timer();
timer.setCallback([]{
    // do some work when the timer fires
});
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
