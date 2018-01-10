# Thread Examples {#threadexamples}

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

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/thread/timer.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Timer examples will come here.