# Thread Examples {#threadexamples}

## ThreadPool

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
var a = 1;
if (a==1) {
	var b = a + 1; 
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


```.cpp
// cpp
var a = 1;
if (a==1) {
	var b = a + 1; 
}
```

```c
// c
var a = 1;
if (a==1) {
	var b = a + 1; 
}
```

```.cpp
// cpp
var a = 1;
if (a==1) {
	var b = a + 1; 
}
```

```cpp
// cpp
var a = 1;
if (a==1) {
	var b = a + 1; 
}
```


```javascript
// javascript
var a = 1;
if (a==1) {
	var b = a + 1; 
}
```

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
#include <ccol/thread/threadpool.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To create a ThreadPool with an optimal amount of Threads:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
ccol::thread::ThreadPool threadpool;
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To create a ThreadPool with a specific amount of Threads:		

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx} 
ccol::thread::ThreadPool threadpool{2}; // create 2 threads on the ThreadPool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To execute on the ThreadPool: 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
ccol::thread::ThreadPool threadpool;		
threadpool.enqueue([]{
	// some code that will run on the ThreadPool
});
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To create a wrapper around a lambda that will run on the ThreadPool each time it is invoked. 
You could use this in combination with a Timer to run Timer events on the ThreadPool, for example.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
ccol::thread::ThreadPool threadpool;		
auto wrappedlambda = threadpool.createWrapper([]{
	// some code that will run on the ThreadPool LATER
});
for (int counter=0; counter<10; counter++) { // will create 10 jobs on the ThreadPool
	wrappedlambda(); // will run on the ThreadPool
}	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	

The amount of jobs waiting to be processed can be retrieved by calling:

~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
threadpool.queueCount();
~~~~~~~~~~~~~~~~~~~~~~~~

The amount of thread available can be retrieved with the following call: 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
ccol::thread::ThreadPool threadpool{2}; // create 2 threads on the ThreadPool
threadpool.threadCount(); // returns 2.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To pause or stop processing you can pull queued jobs from the threadpool. 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
auto jobs = threadpool.dequeue();
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

And to resume the jobs pulled from the threadpool you can enqueue them again.			

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cxx}
auto jobs = threadpool.dequeue();
threadpool.enqueue(jobs);
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## Timer

Timer examples will come here.