#pragma once
#include <deque>
#include "Event.h"
#include "Request.h"

class Worker
{
public:
	double TimeInWork=0.f;
	int ReqAcceptedCount=0;

	Event _event;
    int  ReqState  =-1;
    int  ReqPriority=-1;
	double wf = -1.0;

	static  std::deque<Request> ProcessedReqQueue;
};
