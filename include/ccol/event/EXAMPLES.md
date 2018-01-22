# Event Examples

## EventQueue

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
#include <ccol/event/eventqueue.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Create the EventQueue, add one or more callbacks and run it on thread A.
The events should derive from BaseEvent. When run is called the events
that are enqueued are passed to the resgistered callbacks until stop is
called.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
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
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
// Thread B
using namespace ccol::event;
typedef StaticDataEvent<long> SharedLongEvent;
// Enqueue event SharedLongEvent to the EventQueue. EventQueue
// queue is a reference to the EventQueue in Thread A.
queue.enqueue(std::make_shared<SharedLongEvent>(10));
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Registering multiple event handlers.

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




~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
#include <ccol/event/callbackevent.hxx>
#include <ccol/event/dataevent.hxx>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

