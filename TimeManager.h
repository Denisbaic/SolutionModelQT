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

	//����������� ������������� ������� 
	double GetSystemUtilizationP() const;
	//������� ����� �������� ������ � ������� 
	double GetAverageWaitingTimeForAnApplicationInQueueTq() const;
	//������� ����� ���������� ������ � �������
	double GetAverageTimeSpentByTheApplicationInTheSystemTs() const;
	//������� �� ������� ����� ���������� � �������
	double GetTimeAverageNumberOfRequestsInTheQueueNq() const;
	//������� �� ������� ����� ���������� � ������� 
	double GetTimeAverageNumberOfRequirementsInTheSystemNs() const;
	//���������� ���������� ����������� ������� 
	double GetAbsoluteSystemCapacityCa() const;

	//��������� ��������� �����
	double Exponential_rasp(double med) const;
	//1-������� ���������
	//2-������ ���������
	int	   GetPriorityForRequest();
	int	   GetReqCountInDeq() const;

    void SetGraphicsDataAdmission();
    void SetGraphicsDataService();

	bool TimeEquivalently(double l, double r) const;
};

