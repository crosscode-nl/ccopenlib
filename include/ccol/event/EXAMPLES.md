# Event Examples

## EventQueue

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/event/eventqueue.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create the EventQueue, add one or more callbacks and run it on thread A.
The events should derive from BaseEvent. When run is called the events
that are enqueued are passed to the resgistered callbacks until stop is
called.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
// Thread A
using namespace ccol::event;
EventQueue queue; // The event queue
// type of the event (anything derived from BaseEvent)
typedef StaticDataEvent<long> SharedLongEvent;
queue.setCallbackForType(
    typeid(SharedLongEvent), // typeid of the event
    [](EventQueue::event_type &&event){
        // handle event
    }
);

// start other threads (Thread B) with references to this EventQueue.

queue.run(); // Run the event loop until queue.stop() is called
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
// Thread B
using namespace ccol::event;
typedef StaticDataEvent<long> SharedLongEvent;
// Enqueue event SharedLongEvent to the EventQueue. EventQueue
// queue is a reference to the EventQueue in Thread A.
queue.enqueue(std::make_shared<SharedLongEvent>(10));
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Registering multiple event handlers.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
queue.setCallbacks(
{
    {
        typeid(ccol::event::CallbackEvent),
        [](EventQueue::event_type &&event){
            auto callbackEvent = std::dynamic_pointer_cast<ccol::event::CallbackEvent>(event);
            // handle event
        }
    },
    {
        typeid(IntEvent),
        [](EventQueue::event_type &&event){
            auto dataEvent = std::dynamic_pointer_cast<IntEvent>(event);
            // handle data event            }
    },
    {
        typeid(SharedLongEvent),
        [](EventQueue::event_type &&event){

            auto dataEvent = std::dynamic_pointer_cast<SharedLongEvent>(event);
            // handle data event
        }
    }
});
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To limit the queue pass the maximum queue items to the constructor.
When the queue is full enqueue() will return false.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
// Thread A
EventQueue queue(3); // The event queue
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## StaticDataEvent

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/event/dataevent.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use a static data event to pass a value by copy to the event.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
typedef ccol::event::StaticDataEvent<int> IntEvent;
queue.enqueue(std::make_shared<IntEvent>(-10));
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To retrieve the data in the event handler:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
auto dataEvent = std::dynamic_pointer_cast<IntEvent>(event);
if (dataEvent!=nullptr) {
    EXPECT_EQ(-10,dataEvent->dataRef());
    EXPECT_EQ(-10,dataEvent->dataCopy());
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## SharedDataEvent

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/event/dataevent.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use a SharedDataEvent to pass a value by as a shared_ptr to the event.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
typedef ccol::event::SharedDataEvent<long> SharedLongEvent;
queue.enqueue(std::make_shared<SharedLongEvent>(std::make_shared<long>(18)));
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To retrieve the data in the event handler:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
auto dataEvent = std::dynamic_pointer_cast<SharedLongEvent>(event);
if (dataEvent!=nullptr) {
    EXPECT_EQ(18,*dataEvent->dataRef());
    EXPECT_EQ(18,*dataEvent->dataCopy());
    longCallbackCalled = true;
}
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## CallbackEventQueue

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/event/callbackeventqueue.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create an instance of the CallbackEventQueue.
Use enqueue to schedule a lambda method to the queue.
Run queue by calling run. The queue will now loop until
stop is called. Stop muse be called from another thread or
scheduled as an event before calling run(). A lambda method
the calls stop can also be scheduled from another thread.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
    ccol::event::CallbackEventQueue queue;
    queue.enqueue([&queue]{
        // Do some work.
        queue.stop(); // Call queue stop if you want to stop the queue.
    });
    queue.run(); // Will loop until stop is called.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To limit the queue pass the maximum queue items to the constructor.
When the queue is full enqueue() will return false.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
ccol::event::CallbackEventQueue queue(3);
bool result = queue.enqueue([&queue]{
                // Do some work.
                queue.stop(); // Call queue stop if you want to stop the queue.
              });
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To wrap a lambda method to always run on the queue use wrap().

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
    ccol::event::CallbackEventQueue queue;
    int count = 0;
    auto wrapped = queue.wrap([&queue, &count]{ count++; });
    wrapped();
    wrapped();
    wrapped();
    EXPECT_TRUE(queue.enqueue([&queue, &count]{
        count++;
        queue.enqueue([&queue]{ queue.stop(); }); // placed at the back of the queue, causing the EventQueue to stop.
    }));
    queue.run();
    // count will be 4.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## CallbackEvent

Include header:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~cpp
#include <ccol/event/callbackevent.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This class is used internally by the CallbackEventQueue class.

