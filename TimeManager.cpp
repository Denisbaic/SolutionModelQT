#include "TimeManager.h"
#include <cmath>
#include <iostream>
#include <time.h>
#include <cassert>
#include <QMessageBox>
TimeManager::TimeManager(int req_need,int WorkersCount,  double _AverageServiceTime, double _AverageReqAdmissionTime, bool SetTime) :  PriorityGenerator(Generator(RAND_MAX, 0)),
                                                                                                AverageServiceTime(_AverageServiceTime),
                                                                                                AverageReqAdmissionTime(_AverageReqAdmissionTime),
                                                                                                ReqAdded(0),
                                                                                                ReqNeed(req_need),
                                                                                                CurrentTime(0.f)
{
    if(SetTime){
         srand(time(NULL));
    }

    CountOfWorkers=WorkersCount;
    GroupOfWorkers=new Worker[WorkersCount];

    ExpTime= new QVector<double>(1000);
    double x = 0;
    std::generate(ExpTime->begin(), ExpTime->end(), [&]{ return x+=0.01; });
}

TimeManager::~TimeManager()
{
    delete[] GroupOfWorkers;    
    delete ExpTime;
}

void TimeManager::AddNextReqBeforeSomeTime()
{
	ReqAdded++;
    auto TempTime=Exponential_rasp(AverageReqAdmissionTime);
    TimeHandle.insert(Event(EventDestination::NewReqest, TempTime + CurrentTime));

}

Worker* TimeManager::FindFreeWorker()
{
    for(int i=0;i<CountOfWorkers;i++){
        if(GroupOfWorkers[i].ReqState==-1){
            return &GroupOfWorkers[i];
        }
    }

	return nullptr;
}

Worker* TimeManager::FindWorkerWithLowPriority()
{
    for(int i=0;i<CountOfWorkers;i++){
        if(GroupOfWorkers[i].ReqState==2){
            return &GroupOfWorkers[i];
        }
    }
	return nullptr;
}


//New
void TimeManager::UseFreeWorkerWithNewReq(Worker *w, int ReqPriority, bool isReqAbsolute)
{
    const double TimeTemp = Exponential_rasp(AverageServiceTime);
    w->wf = TimeTemp + CurrentTime;
    w->TimeInWork += TimeTemp;
    w->ReqState = isReqAbsolute ? 1 : 2;
    //w->_ReqPriority.absolutePriority=isReqAbsolute;
    w->ReqPriority=ReqPriority;

    w->_event = Event(EventDestination::SomeRequestTimeEnd, w->wf, w, Request(TimeTemp, ReqPriority,isReqAbsolute, CurrentTime,CurrentTime, 0));

    TimeHandle.insert(w->_event);
}


//New
void TimeManager::UseBusyWorkerWithNewReq(Worker *w, int ReqPriority, bool isReqAbsolute)
{
    if(w->ReqState==1 && isReqAbsolute){
        if(GetReqCountInDeq()>=Limit)
        {
            if(!ReqDeqPriority2.empty()){
                ReqDeqPriority2[FindLowestReq(ReqDeqPriority2)].ExitTime=CurrentTime;

                ReqFailed.push_back(ReqDeqPriority2[FindLowestReq(ReqDeqPriority2)]);
                ReqDeqPriority2.erase(ReqDeqPriority2.begin()+FindLowestReq(ReqDeqPriority2));

            }
            else{
                if(ReqDeqPriority1[FindLowestReq(ReqDeqPriority1)].ReqPriority>w->ReqPriority){
                    w->_event.req.ExitTime=CurrentTime;
                    ReqFailed.push_back(w->_event.req);
                }else{
                    ReqDeqPriority1[FindLowestReq(ReqDeqPriority1)].ExitTime=CurrentTime;
                    ReqDeqPriority1.erase(ReqDeqPriority1.begin()+FindLowestReq(ReqDeqPriority1));
                    ReqFailed.push_back(ReqDeqPriority1[FindLowestReq(ReqDeqPriority1)]);

                    ReqDeqPriority1.push_back( w->_event.req);
                }
            }
        }
        else{
            ReqDeqPriority1.push_back( w->_event.req);
        }
    }
    else if(w->ReqState==2 && isReqAbsolute){
        //���������� ������ � �� ���������� �����������
        //ReqFailed.push_back(w->_event.req);
        ReqDeqPriority2.push_front(w->_event.req);
    }
    else if(w->ReqState==2 && !isReqAbsolute){
        if(GetReqCountInDeq()>=Limit)
        {
            if(ReqDeqPriority2[FindLowestReq(ReqDeqPriority2)].ReqPriority<ReqPriority){
                ReqFailed.push_back(ReqDeqPriority2[FindLowestReq(ReqDeqPriority2)]);
                ReqDeqPriority2.erase(ReqDeqPriority2.begin()+FindLowestReq(ReqDeqPriority2));

                ReqDeqPriority2.push_back( w->_event.req);
            }
            else{
                w->_event.req.ExitTime=CurrentTime;
                ReqFailed.push_back(w->_event.req);
            }
        }else{
            ReqDeqPriority2.push_back(w->_event.req);
        }
    }
    else if(w->ReqState==1 && !isReqAbsolute){
        QMessageBox msgBox;
        msgBox.setText("Something is wrong. Func UseBusyWorkerWithNewReq try to change state from absolute to non absolute.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();

    }

    TimeHandle.erase(w->_event);
    w->TimeInWork -= w->wf - CurrentTime;
    const double TimeTemp = Exponential_rasp(AverageServiceTime);
    w->wf = TimeTemp + CurrentTime;
    w->TimeInWork += TimeTemp;
    w->_event = Event(SomeRequestTimeEnd, w->wf, w, Request(TimeTemp, ReqPriority, isReqAbsolute, CurrentTime,CurrentTime, 0));
    w->ReqState = isReqAbsolute ? 1 : 2;
    w->ReqPriority=ReqPriority;

    TimeHandle.insert(w->_event);
}


void TimeManager::UseFreeWorkerWithReqFromDeq(Worker *w, std::deque<Request> *ReqDeq, bool isReqAbsolute)
{    
/*
    if(!isReqAbsolute){
        ReqFromDeq = *ReqDeq->begin();
        ReqFromDeq.WorkBeginTime = CurrentTime;
        ReqDeq->pop_front();
    }
    else{
        int TempIndex=FindHighestReq(*ReqDeq);
        ReqFromDeq=*(ReqDeq->begin()+TempIndex);
        ReqDeq->erase(ReqDeq->begin()+TempIndex);
    }
*/
    int TempIndex=FindHighestReq(*ReqDeq);
    Request ReqFromDeq=*(ReqDeq->begin()+TempIndex);
    ReqDeq->erase(ReqDeq->begin()+TempIndex);

    w->wf = ReqFromDeq.WorkTime + CurrentTime;
    w->ReqState=isReqAbsolute ? 1 : 2;
    w->ReqPriority = ReqFromDeq.ReqPriority;
    w->TimeInWork += ReqFromDeq.WorkTime;

    w->_event = Event(SomeRequestTimeEnd, w->wf, w, ReqFromDeq);
    TimeHandle.insert(w->_event);
}


//New
bool TimeManager::TryToPushReqWithHighPriority(int ReqPriority)
{
    //TODO возможно, следует убирать заявку с самым низким абсолютным приоритетом
    if(ReqDeqPriority2.empty())
        return false;
    ReqFailed.push_back(*ReqDeqPriority2.rbegin());
    ReqDeqPriority2.pop_back();


    ReqDeqPriority1.push_back(Request(Exponential_rasp(AverageServiceTime), ReqPriority, true, 0.0, CurrentTime, 0.0));
    return true;
}

int TimeManager::FindLowestReq(std::deque<Request> &ReqDeq)
{
    int i=0;
    int iOut=0;
    int MinReqPriority=-1;
    for(auto& elem : ReqDeq){
        if(MinReqPriority>elem.ReqPriority)
         {
            MinReqPriority=elem.ReqPriority;
            iOut=i;
         }
        i+=1;
    }
    return iOut;
}

int TimeManager::FindHighestReq(std::deque<Request> &ReqDeq)
{
    int i=0;
    int iOut=0;
    int MaxReqPriority=-1;
    for(auto& elem : ReqDeq){
        if(MaxReqPriority<elem.ReqPriority)
         {
            MaxReqPriority=elem.ReqPriority;
            iOut=i;
         }
        i+=1;
    }
    return iOut;
}

Worker *TimeManager::FindLowestPriorityWorker(bool isReqAbsolute)
{
    Worker* TempMin=nullptr;
    if(isReqAbsolute){
        for(int i=0; i<CountOfWorkers; ++i){
            if(GroupOfWorkers[i].ReqState==1)
            {
                TempMin= &GroupOfWorkers[i];
                break;
            }
        }
        for(int i=0; i<CountOfWorkers; ++i){
            if(GroupOfWorkers[i].ReqState==1 && GroupOfWorkers[i].ReqPriority<TempMin->ReqPriority)
            {
                TempMin= &GroupOfWorkers[i];
            }
        }
    }
    else{
        for(int i=0; i<CountOfWorkers; ++i){
            if(GroupOfWorkers[i].ReqState==2)
            {
                TempMin= &GroupOfWorkers[i];
                break;
            }
        }
        for(int i=0; i<CountOfWorkers; ++i){
            if(GroupOfWorkers[i].ReqState==2 && GroupOfWorkers[i].ReqPriority<TempMin->ReqPriority)
            {
                TempMin= &GroupOfWorkers[i];
            }
        }
    }
    return TempMin;
}

void TimeManager::RequestInWork()
{
    const bool requestType=GetTypeForRequest();
    const int  requestPriority=GetPriorityForRequest(requestType);

    if(requestType){
        if(ReqDeqPriority1.empty()){
            Worker* WorkerPtr = FindFreeWorker();
            if(WorkerPtr)
            {
                UseFreeWorkerWithNewReq(WorkerPtr, requestPriority,true);
            }
            else{
                WorkerPtr = FindWorkerWithLowPriority();
                if(WorkerPtr){
                    UseBusyWorkerWithNewReq(WorkerPtr,requestPriority,true);
                }
                else{
                    WorkerPtr=FindLowestPriorityWorker(true);
                    if(WorkerPtr->ReqPriority<requestPriority){
                        UseBusyWorkerWithNewReq(WorkerPtr,requestPriority,true);
                    }
                    else{
                        if (GetReqCountInDeq() >= Limit)
                        {
                            TryToPushReqWithHighPriority(requestPriority);
                        }
                        else{
                            ReqDeqPriority1.push_back(Request(Exponential_rasp(AverageServiceTime),requestPriority, true,CurrentTime,CurrentTime,CurrentTime));
                        }
                    }
                }
            }
        }
        else{
            if (GetReqCountInDeq() >= Limit)
            {
                if(!TryToPushReqWithHighPriority(requestPriority))
                {
                    ReqFailed.push_back(Request(0.0, requestPriority,true,CurrentTime, CurrentTime, CurrentTime));
                }
            }
            else
            {
                ReqDeqPriority1.emplace_back(Exponential_rasp(AverageServiceTime),requestPriority,true,CurrentTime, CurrentTime, 0.0);
            }
            //TryToPushReqWithHighPriority(requestPriority);
        }
    }
    else{
        if(ReqDeqPriority2.empty())
        {
            Worker* WorkerPtr = FindFreeWorker();
            if (WorkerPtr)
            {
                UseFreeWorkerWithNewReq(WorkerPtr, requestPriority,false);
                //UseFreeWorkerWithNewReq(WorkerPtr, 2);
            }
            else
            {
                WorkerPtr=FindLowestPriorityWorker(false);
                if(WorkerPtr && WorkerPtr->ReqPriority<requestPriority){
                    UseBusyWorkerWithNewReq(WorkerPtr,requestPriority,false);
                }
                else{
                    if (GetReqCountInDeq() >= Limit)
                    {
                        ReqFailed.push_back(Request(0.0, requestPriority,false, CurrentTime,CurrentTime, CurrentTime));
                    }
                    else
                    {
                        ReqDeqPriority2.push_back(Request(Exponential_rasp(AverageServiceTime),requestPriority,false,0.0, CurrentTime, 0.0));
                    }
                }
            }
        }
        else
        {
            //TODO Make Swither
            if (GetReqCountInDeq() >= Limit)
            {
                ReqFailed.push_back(Request(0.0,requestPriority, false, CurrentTime, CurrentTime, CurrentTime));
            }
            else
            {
                ReqDeqPriority2.push_back(Request(Exponential_rasp(AverageServiceTime), requestPriority, false, 0.0,CurrentTime, 0.0));
            }

        }
    }


	//������������� ��� �������
	CheckReqContainer();
}

void TimeManager::CheckReqContainer()
{
	//check on free workers
	Worker* workerPtr = nullptr;
	workerPtr=FindFreeWorker();
	while (!ReqDeqPriority1.empty() && workerPtr)
	{
        UseFreeWorkerWithReqFromDeq(workerPtr,&ReqDeqPriority1,true);
		workerPtr = FindFreeWorker();
	}
	//check on low priority
	workerPtr = nullptr;
	workerPtr = FindWorkerWithLowPriority();
	while (!ReqDeqPriority1.empty() && workerPtr)
	{
        //UseBusyWorkerWithReqFromDeq(workerPtr, &ReqDeqPriority1, true);

        ReqDeqPriority2.push_back(workerPtr->_event.req);
        TimeHandle.erase(workerPtr->_event);
        workerPtr->TimeInWork -= workerPtr->wf - CurrentTime;

        //auto ReqFromDeq = *ReqDeqPriority1.begin();
        Request ReqFromDeq=ReqDeqPriority1[FindHighestReq(ReqDeqPriority1)];
        ReqFromDeq.WorkBeginTime = CurrentTime;
        ReqDeqPriority1.erase(ReqDeqPriority1.begin()+FindHighestReq(ReqDeqPriority1));

        workerPtr->wf = ReqFromDeq.WorkTime + CurrentTime;
        workerPtr->ReqState = 1;
        workerPtr->ReqPriority=ReqFromDeq.ReqPriority;
        workerPtr->TimeInWork += ReqFromDeq.WorkTime;

        workerPtr->_event = Event(SomeRequestTimeEnd, workerPtr->wf, workerPtr, ReqFromDeq);
        TimeHandle.insert(workerPtr->_event);
		workerPtr = FindWorkerWithLowPriority();
	}

    //TODO make func for this
    workerPtr = nullptr;
    workerPtr = FindLowestPriorityWorker(true);
    while(!ReqDeqPriority1.empty() && ReqDeqPriority1[FindHighestReq(ReqDeqPriority1)].ReqPriority>workerPtr->ReqPriority){
        ReqDeqPriority1.push_back(workerPtr->_event.req);
        TimeHandle.erase(workerPtr->_event);
        workerPtr->TimeInWork -= workerPtr->wf - CurrentTime;

        Request ReqFromDeq=ReqDeqPriority1[FindHighestReq(ReqDeqPriority1)];
        ReqFromDeq.WorkBeginTime = CurrentTime;
        ReqDeqPriority1.erase(ReqDeqPriority1.begin()+FindHighestReq(ReqDeqPriority1));

        workerPtr->wf = ReqFromDeq.WorkTime + CurrentTime;
        workerPtr->ReqState = 1;
        workerPtr->ReqPriority=ReqFromDeq.ReqPriority;
        workerPtr->TimeInWork += ReqFromDeq.WorkTime;

        workerPtr->_event = Event(SomeRequestTimeEnd, workerPtr->wf, workerPtr, ReqFromDeq);
        TimeHandle.insert(workerPtr->_event);
        workerPtr = FindLowestPriorityWorker(true);
    }

	//check free workers for low priority requests
	workerPtr = nullptr;
	workerPtr = FindFreeWorker();
	while (!ReqDeqPriority2.empty() && workerPtr)
	{
        //UseFreeWorkerWithReqFromDeq(workerPtr, &ReqDeqPriority2,2);
        UseFreeWorkerWithReqFromDeq(workerPtr,&ReqDeqPriority2,false);
		workerPtr = FindFreeWorker();
	}


}

Event TimeManager::MoveTime()
{
    Time.push_back(CurrentTime);
    ReqCountPerFix.push_back(GetReqCountInDeq());
    ReqCountInSystemPerFix.push_back(GetReqCountInSystem());

    NsPerFix.push_back(GetTimeAverageNumberOfRequirementsInTheSystemNs());
    NqPerFix.push_back(GetTimeAverageNumberOfRequestsInTheQueueNq());

    PPerFix.push_back(GetSystemUtilizationP());
	const auto TempEvent = (*TimeHandle.begin());
	CurrentTime = TempEvent.time;	

    //std::cout << CurrentTime<<" ";
    //std::cout << TempEvent.time << " ";
	
//	switch (TempEvent.event_destination)
//	{
//	case NewReqest: std::cout << " New request" << std::endl; break;
//	case SomeRequestTimeEnd:std::cout << " SomeRequestTimeEnd" << std::endl; break;
//	default: ;
//	}

	TimeHandle.erase(TimeHandle.begin());


	return TempEvent;
}

double TimeManager::GetSystemUtilizationP() const
{
    if(CurrentTime==0)
        return 0.0;
    double Temp=0.0;

    for(int i=0;i<CountOfWorkers;i++){
        Temp+=GroupOfWorkers[i].TimeInWork;
    }
    return Temp/ (CurrentTime * CountOfWorkers);
}

double TimeManager::GetAverageWaitingTimeForAnApplicationInQueueTq() const
{
    if(Worker::ProcessedReqQueue.empty())
        return 0.0;
	auto TempTime = 0.0;
	for(auto& elem : Worker::ProcessedReqQueue)
	{
		TempTime += elem.ExitTime - elem.ArrivalTime - elem.WorkTime;
	}
    return TempTime / (Worker::ProcessedReqQueue.size());
}

double TimeManager::GetAverageTimeSpentByTheApplicationInTheSystemTs() const
{
    //TODO �������� �� ���������
    //���� ���, �������� : return GetAverageWaitingTimeForAnApplicationInQueueTq() + AverageServiceTime;
    return GetAverageWaitingTimeForAnApplicationInQueueTq() + 1.0/AverageServiceTime;
}

double TimeManager::GetTimeAverageNumberOfRequestsInTheQueueNq() const
{
    //TODO �������� �� ���������
    //���� ���, �������� : return GetAverageWaitingTimeForAnApplicationInQueueTq() / AverageServiceTime;
    return GetAverageWaitingTimeForAnApplicationInQueueTq() * AverageReqAdmissionTime;
}

double TimeManager::GetTimeAverageNumberOfRequirementsInTheSystemNs() const
{
    //TODO �������� �� ���������
    //���� ���, �������� : return GetAverageWaitingTimeForAnApplicationInQueueTs() / AverageServiceTime;
    return GetAverageTimeSpentByTheApplicationInTheSystemTs() * AverageReqAdmissionTime;
}

double TimeManager::GetAbsoluteSystemCapacityCa() const
{
    return Worker::ProcessedReqQueue.size() / CurrentTime;
}

double TimeManager::GetAbsoluteSystemCapacityCr() const
{
    return double(Worker::ProcessedReqQueue.size())/(Worker::ProcessedReqQueue.size()+ReqFailed.size());
}

double TimeManager::GetI() const
{//TODO доделать
    return -1.0;
}

double TimeManager::Exponential_rasp(double med) const
{
	const auto y = double(rand()) / RAND_MAX; //���������� �������������� ��������� �������� [0,1]
	return -1 / med * log(y);
}


bool TimeManager::GetTypeForRequest()
{
	double GeneratedValue = PriorityGenerator.next();

	GeneratedValue = GeneratedValue / RAND_MAX;
    if (GeneratedValue < TypeRequestBound)
        return true;
    return false;
}

int TimeManager::GetPriorityForRequest(bool isRequestAbsolute){

    double GeneratedValue = PriorityGenerator.next();

    GeneratedValue = GeneratedValue / RAND_MAX;
    if(isRequestAbsolute){
        return FindPriorityInMap(HighPriorityArr,GeneratedValue);
    }
    else{
        return FindPriorityInMap(LowPriorityArr,GeneratedValue);
    }
}

int TimeManager::FindPriorityInMap(QMap<int, QPair<double, double> > &PriorityMap, double Priority)
{
    for(auto i=PriorityMap.begin(); i!=PriorityMap.end();++i){
        if(i.value().first<=Priority && i.value().second>=Priority){
            return i.key();
        }
    }
    return -1;
}

int TimeManager::GetReqCountInDeq() const
{
    return ReqDeqPriority1.size() + ReqDeqPriority2.size();
}

int TimeManager::GetReqCountInSystem() const
{
    int TempCount=GetReqCountInDeq();

    for(int i=0;i<CountOfWorkers;i++){
        if(GroupOfWorkers[i].ReqState!=-1)
            TempCount++;
    }

    return TempCount;
}


void TimeManager::SetGraphicsDataExpAdmission()
{
    for(int i=0; i<ExpTime->size();++i)
        ExpRaspAdmission.push_back(1-std::pow(2.71828,-AverageReqAdmissionTime*i));
}

void TimeManager::SetGraphicsDataExpService()
{
    for(int i=0; i<ExpTime->size();++i)
       ExpRaspService.push_back(1-std::pow(2.71828,-AverageServiceTime*i));
}

void TimeManager::SetGraphicsDataExpDensityAdmission()
{
    for(int i=0; i<ExpTime->size();++i)
        ExpRaspDensityAdmission.push_back(AverageReqAdmissionTime*std::pow(2.71828,-AverageReqAdmissionTime*i));
}

void TimeManager::SetGraphicsDataExpDensityService()
{
    for(int i=0; i<ExpTime->size();++i)
        ExpRaspDensityService.push_back(AverageServiceTime*std::pow(2.71828,-AverageServiceTime*i));
}

bool TimeManager::TimeEquivalently(double l, double r)
{
    return std::abs(r - l) < 0.001f;
}
