#include "TimeManager.h"
#include <cmath>
#include <iostream>
#include <time.h>

TimeManager::TimeManager(int req_need, double _AverageServiceTime, double _AverageReqAdmissionTime) :  PriorityGenerator(Generator(RAND_MAX, 0)),
                                                                                                AverageServiceTime(_AverageServiceTime),
                                                                                                AverageReqAdmissionTime(_AverageReqAdmissionTime),
                                                                                                ReqAdded(0),
                                                                                                ReqNeed(req_need),
                                                                                                CurrentTime(0.f)
{
	srand(time(NULL));
}

void TimeManager::AddNextReqBeforeSomeTime()
{
	ReqAdded++;
	TimeHandle.insert(Event(EventDestination::NewReqest, Exponential_rasp(AverageReqAdmissionTime) + CurrentTime));
}

Worker* TimeManager::FindFreeWorker()
{
	if(w1.ReqPriority==-1)
	{
		return &w1;
	}
	if(w2.ReqPriority==-1)
	{
		return &w2;
	}
	if (w3.ReqPriority == -1)
	{
		return &w3;
	}
	if (w4.ReqPriority == -1)
	{
		return &w4;
	}
	if (w5.ReqPriority == -1)
	{
		return &w5;
	}
	return nullptr;
}

Worker* TimeManager::FindWorkerWithLowPriority()
{
	if (w1.ReqPriority == 2)
	{
		return &w1;
	}
	if (w2.ReqPriority == 2)
	{
		return &w2;
	}
	if (w3.ReqPriority == 2)
	{
		return &w3;
	}
	if (w4.ReqPriority == 2)
	{
		return &w4;
	}
	if (w5.ReqPriority == 2)
	{
		return &w5;
	}
	return nullptr;
}

void TimeManager::UseFreeWorkerWithNewReq(Worker* w, int ReqPriority)
{
	const double TimeTemp = Exponential_rasp(AverageServiceTime);
	w->wf = TimeTemp + CurrentTime;
	w->TimeInWork += TimeTemp;
	w->ReqPriority = ReqPriority;
	w->_event = Event(EventDestination::SomeRequestTimeEnd, w->wf, w, Request(TimeTemp, CurrentTime,CurrentTime, 0));

	TimeHandle.insert(w->_event);
}

void TimeManager::UseBusyWorkerWithNewReq(Worker* w)
{
	if(GetReqCountInDeq()>=Limit)
	{
		const Request TempReq= *ReqDeqPriority2.rbegin();
		ReqDeqPriority2.pop_back();
		ReqFailed.push_back(TempReq);
	}
	ReqDeqPriority2.push_front(w->_event.req);
	TimeHandle.erase(w->_event);
	w->TimeInWork -= w->wf - CurrentTime;

	const double TimeTemp = Exponential_rasp(AverageServiceTime);
	w->wf = TimeTemp + CurrentTime;
	w->TimeInWork += TimeTemp;
	w->_event = Event(SomeRequestTimeEnd, w->wf, w, Request(TimeTemp, CurrentTime,CurrentTime, 0));
	w->ReqPriority = 1;
	TimeHandle.insert(w->_event);
}

void TimeManager::UseFreeWorkerWithReqFromDeq(Worker* w, std::deque<Request>* ReqDeq,int ReqPriority)
{
	auto ReqFromDeq = *ReqDeq->begin();
	ReqFromDeq.WorkBeginTime = CurrentTime;
	ReqDeq->pop_front();

	w->wf = ReqFromDeq.WorkTime + CurrentTime;
	w->ReqPriority = ReqPriority;
	w->TimeInWork += ReqFromDeq.WorkTime;
	
	w->_event = Event(SomeRequestTimeEnd, w->wf, w, ReqFromDeq);
	TimeHandle.insert(w->_event);
}

void TimeManager::UseBusyWorkerWithReqFromDeq(Worker* w, std::deque<Request>* ReqDeq)
{
	ReqDeqPriority2.push_front(w->_event.req);
	TimeHandle.erase(w->_event);
	w->TimeInWork -= w->wf - CurrentTime;

	auto ReqFromDeq = *ReqDeq->begin();
	ReqFromDeq.WorkBeginTime = CurrentTime;
	ReqDeq->pop_front();

	w->wf = ReqFromDeq.WorkTime + CurrentTime;
	w->ReqPriority = 1;
	w->TimeInWork += ReqFromDeq.WorkTime;

	w->_event = Event(SomeRequestTimeEnd, w->wf, w, ReqFromDeq);
	TimeHandle.insert(w->_event);
}

bool TimeManager::TryToPushReqWithHighPriority()
{
	if(ReqDeqPriority2.empty())
		return false;
	ReqFailed.push_back(*ReqDeqPriority2.rbegin());
	ReqDeqPriority2.pop_back();


	ReqDeqPriority1.emplace_back(Exponential_rasp(AverageServiceTime), CurrentTime, CurrentTime, 0.0);
	return true;
}

void TimeManager::RequestInWork()
{
	const int requestPriority = GetPriorityForRequest();

	if(requestPriority==1)
	{
		if(ReqDeqPriority1.empty())
		{
			Worker* WorkerPtr = FindFreeWorker();
			if(WorkerPtr)
			{
				UseFreeWorkerWithNewReq(WorkerPtr, 1);
			}
			else
			{
				WorkerPtr = FindWorkerWithLowPriority();
				if(WorkerPtr)
				{
					UseBusyWorkerWithNewReq(WorkerPtr);
				}
				else
				{
					ReqDeqPriority1.emplace_back(Exponential_rasp(AverageServiceTime), CurrentTime, 0);
				}
			}
		}
		else
		{
			if (GetReqCountInDeq() >= Limit)
			{
				//bool ReqPushed = TryToPushReqWithHighPriority();
				if(!TryToPushReqWithHighPriority())
				{
					ReqFailed.emplace_back(Exponential_rasp(AverageServiceTime), CurrentTime, 0.0);
				}
			}
			else
			{
				ReqDeqPriority1.emplace_back(Exponential_rasp(AverageServiceTime), CurrentTime, 0.0);
			}			
		}
	}
	else
	{
		if(ReqDeqPriority2.empty())
		{
			Worker* WorkerPtr = FindFreeWorker();
			if (WorkerPtr)
			{
				UseFreeWorkerWithNewReq(WorkerPtr, 2);
			}
			else
			{
				ReqDeqPriority2.emplace_back(Exponential_rasp(AverageServiceTime), CurrentTime, 0.0);
			}
		}
		else
		{
			if (GetReqCountInDeq() >= Limit)
			{
				ReqFailed.emplace_back(Exponential_rasp(AverageServiceTime), CurrentTime, 0.0);
			}
			else
			{
				ReqDeqPriority2.emplace_back(Exponential_rasp(AverageServiceTime), CurrentTime, 0.0);
			}
			
		}
	}

	//задействовать все девайсы
	CheckReqContainer();
}

void TimeManager::CheckReqContainer()
{
	//check on free workers
	Worker* workerPtr = nullptr;
	workerPtr=FindFreeWorker();
	while (!ReqDeqPriority1.empty() && workerPtr)
	{
		UseFreeWorkerWithReqFromDeq(workerPtr,&ReqDeqPriority1,1);
		workerPtr = FindFreeWorker();
	}
	//check on low priority
	workerPtr = nullptr;
	workerPtr = FindWorkerWithLowPriority();
	while (!ReqDeqPriority1.empty() && workerPtr)
	{
		UseBusyWorkerWithReqFromDeq(workerPtr, &ReqDeqPriority1);
		workerPtr = FindWorkerWithLowPriority();
	}
	//check free workers for low priority requests
	workerPtr = nullptr;
	workerPtr = FindFreeWorker();
	while (!ReqDeqPriority2.empty() && workerPtr)
	{
		UseFreeWorkerWithReqFromDeq(workerPtr, &ReqDeqPriority2,2);
		workerPtr = FindFreeWorker();
	}


}

Event TimeManager::MoveTime()
{
    Time.push_back(CurrentTime);
    ReqCountPerFix.push_back(GetReqCountInDeq());
    PPerFix.push_back(GetSystemUtilizationP());

	const auto TempEvent = (*TimeHandle.begin());
	CurrentTime = TempEvent.time;
	
	std::cout << CurrentTime<<" ";
	std::cout << TempEvent.time << " ";
	
	switch (TempEvent.event_destination)
	{
	case NewReqest: std::cout << " New request" << std::endl; break;
	case SomeRequestTimeEnd:std::cout << " SomeRequestTimeEnd" << std::endl; break;
	default: ;
	}

	TimeHandle.erase(TimeHandle.begin());

	if(TimeEquivalently(w1.wf,CurrentTime))
	{
		w1.wf = -1.0;
	}
	else if (TimeEquivalently(w2.wf, CurrentTime))
	{
		w2.wf = -1.0;
	}
	return TempEvent;
}

Worker* TimeManager::FindWorkerByTime(double Time)
{
	if (TimeEquivalently(w1.wf, Time))
		return &w1;
	if (TimeEquivalently(w2.wf, Time))
		return &w2;

	return nullptr;
}

double TimeManager::GetSystemUtilizationP() const
{
	return (w1.TimeInWork + w2.TimeInWork+w3.TimeInWork+w4.TimeInWork+w5.TimeInWork) / (CurrentTime * 5);
}

double TimeManager::GetAverageWaitingTimeForAnApplicationInQueueTq() const
{
	auto TempTime = 0.0;
	for(auto& elem : Worker::ProcessedReqQueue)
	{
		TempTime += elem.ExitTime - elem.ArrivalTime - elem.WorkTime;
	}
    return TempTime / (Worker::ProcessedReqQueue.size());
}

double TimeManager::GetAverageTimeSpentByTheApplicationInTheSystemTs() const
{
    //TODO возможно не правильно
    //если что, заменить : return GetAverageWaitingTimeForAnApplicationInQueueTq() + AverageServiceTime;
    return GetAverageWaitingTimeForAnApplicationInQueueTq() + 1.0/AverageServiceTime;
}

double TimeManager::GetTimeAverageNumberOfRequestsInTheQueueNq() const
{
    //TODO возможно не правильно
    //если что, заменить : return GetAverageWaitingTimeForAnApplicationInQueueTq() / AverageServiceTime;
    return GetAverageWaitingTimeForAnApplicationInQueueTq() * AverageReqAdmissionTime;
}

double TimeManager::GetTimeAverageNumberOfRequirementsInTheSystemNs() const
{
    //TODO возможно не правильно
    //если что, заменить : return GetAverageWaitingTimeForAnApplicationInQueueTs() / AverageServiceTime;
    return GetAverageTimeSpentByTheApplicationInTheSystemTs() * AverageReqAdmissionTime;
}

double TimeManager::GetAbsoluteSystemCapacityCa() const
{
    return (w1.TimeInWork + w2.TimeInWork+w3.TimeInWork+w4.TimeInWork+w5.TimeInWork) / CurrentTime;
}

double TimeManager::Exponential_rasp(double med) const
{
	const auto y = double(rand()) / RAND_MAX; //равномерно распределенная случайная величина [0,1]
	return -1 / med * log(y);
}


int TimeManager::GetPriorityForRequest()
{
	double GeneratedValue = PriorityGenerator.next();

	GeneratedValue = GeneratedValue / RAND_MAX;
	if (GeneratedValue > 0.8)
		return 1;
	return 2;
}

int TimeManager::GetReqCountInDeq() const
{
	return ReqDeqPriority1.size() + ReqDeqPriority2.size();
}

bool TimeManager::TimeEquivalently(double l, double r) const
{
    return std::abs(r - l) < 0.001f;
}
