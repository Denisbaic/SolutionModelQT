#include "Event.h"

Event::Event(EventDestination event_destination, double time, Worker* _workerPtr, Request _req): event_destination(event_destination),
time(time)
, workerPtr(_workerPtr),
req(_req)
{
}

Event::Event(EventDestination event_destination, double time, Worker* _workerPtr)
	: event_destination(event_destination),
	time(time)
	,workerPtr(_workerPtr)
{
}

Event::Event(EventDestination event_destination, double time) : event_destination(event_destination),
time(time),
workerPtr(nullptr)
{
}

Event::Event(): event_destination(), time(0), workerPtr(nullptr)
{
}

bool Event::operator<( Event const& R) const
{
	return  this->time< R.time;
}

