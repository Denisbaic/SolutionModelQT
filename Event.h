#pragma once
#include "Request.h"

enum EventDestination
{
	NewReqest, SomeRequestTimeEnd
};
class Event
{
public:
	EventDestination event_destination;
	double time;
	class Worker* workerPtr;

	Request req;
	Event(EventDestination event_destination, double time, Worker* _workerPtr, Request _req);
	Event(EventDestination event_destination, double time, Worker* _workerPtr);
	Event(EventDestination event_destination, double time);
	Event();
	bool operator<(Event const  & R) const;
    bool operator==(Event const& R);
};

