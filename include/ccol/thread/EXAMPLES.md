# Thread Examples {#threadexamples}

## ThreadPool

Include header:
		
		#include <ccol/thread/threadpool.hxx>

To create a ThreadPool with an optimal amount of Threads:

		ccol::thread::ThreadPool threadpool;

To create a ThreadPool with an custom amount of Threads:		

		ccol::thread::ThreadPool threadpool{2}; // create 2 threads on the ThreadPool

To execute on the ThreadPool: 

		ccol::thread::ThreadPool threadpool;		
		threadpool.enqueueJob([]{
			// some code that will run on the ThreadPool
		});

To create a wrapper around a lambda that will run on the ThreadPool each time it is invoked. 
You could use this in combination with a Timer to run Timer events on the ThreadPool, for example.

		ccol::thread::ThreadPool threadpool;		
		auto wrappedlambda = threadpool.createWrapper([]{
			// some code that will run on the ThreadPool LATER
		});
		for (int counter=0; counter<10; counter++) { // will create 10 jobs on the ThreadPool
			wrappedlambda(); // will run on the ThreadPool
		}		


## Timer

Timer examples will come here.