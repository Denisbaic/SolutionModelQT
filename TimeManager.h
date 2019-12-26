#pragma once
#include <set>
#include "Generator.h"
#include "Event.h"
#include "Worker.h"
#include <QPair>
#include <QVector>
class TimeManager
{
	Generator PriorityGenerator;

	std::deque<Request> ReqDeqPriority1;
	std::deque<Request> ReqDeqPriority2;



	Worker* FindFreeWorker();
	Worker* FindWorkerWithLowPriority();
	void UseFreeWorkerWithNewReq(Worker* w, int ReqPriority);
	void UseBusyWorkerWithNewReq(Worker* w);

	void UseFreeWorkerWithReqFromDeq(Worker* w, std::deque<Request>* ReqDeq, int ReqPriority);
	void UseBusyWorkerWithReqFromDeq(Worker* w, std::deque<Request>* ReqDeq);

	bool TryToPushReqWithHighPriority();

public:
	TimeManager(int req_need, double _AverageServiceTime, double _AverageReqAdmissionTime);
	double AverageServiceTime;
	double AverageReqAdmissionTime;
	int ReqAdded = 0;
	int ReqNeed = 100;
	int Limit=26;
	std::multiset<Event> TimeHandle;

    std::deque<Request> ReqFailed;
    ////GRAPHIC DATA///
    QVector<double> ExpRaspAdmission; //QVector<double> ExpTimeA;
    QVector<double> ExpRaspService;   //QVector<double> ExpTimeS;

    QVector<double> NsPerFix;
    QVector<double> NqPerFix;
    QVector<double> PPerFix;
    QVector<double> ReqCountPerFix;
    QVector<double> Time;
    //////////////

	Worker w1;
	Worker w2;
	Worker w3;
	Worker w4;
	Worker w5;

	double CurrentTime = 0.f;

	void AddNextReqBeforeSomeTime();

	void RequestInWork();
	void CheckReqContainer();
	Event MoveTime();

	Worker* FindWorkerByTime(double Time);

	//коэффициент использования системы 
	double GetSystemUtilizationP() const;
	//среднее время ожидания заявки в очереди 
	double GetAverageWaitingTimeForAnApplicationInQueueTq() const;
	//среднее время пребывания заявки в системе
	double GetAverageTimeSpentByTheApplicationInTheSystemTs() const;
	//среднее по времени число требований в очереди
	double GetTimeAverageNumberOfRequestsInTheQueueNq() const;
	//среднее по времени число требований в системе 
	double GetTimeAverageNumberOfRequirementsInTheSystemNs() const;
	//абсолютная пропускная способность системы 
	double GetAbsoluteSystemCapacityCa() const;

	//генератор случайных чисел
	double Exponential_rasp(double med) const;
	//1-высокий приоритет
	//2-низкий приоритет
	int	   GetPriorityForRequest();
	int	   GetReqCountInDeq() const;

    void SetGraphicsDataAdmission();
    void SetGraphicsDataService();

	bool TimeEquivalently(double l, double r) const;
};

