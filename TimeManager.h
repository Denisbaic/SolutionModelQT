#pragma once
#include <set>
#include "Generator.h"
#include "Event.h"
#include "Worker.h"
#include <QPair>
#include <QVector>
#include <QMap>
#include <QPair>

class TimeManager
{
	Generator PriorityGenerator;

    std::deque<Request> ReqDeqPriority1;
    std::deque<Request> ReqDeqPriority2;

	Worker* FindFreeWorker();
	Worker* FindWorkerWithLowPriority();
    void    UseFreeWorkerWithNewReq(Worker* w, int ReqPriority, bool isReqAbsolute);
    void    UseBusyWorkerWithNewReq(Worker* w,int ReqPriority, bool isReqAbsolute);


    void    UseFreeWorkerWithReqFromDeq(Worker* w, std::deque<Request>* ReqDeq, bool isReqAbsolute);
    //void    UseBusyWorkerWithReqFromDeq(Worker* w, std::deque<Request>* ReqDeq, bool isReqAbsolute);

    bool    TryToPushReqWithHighPriority(int ReqPriority);

    int     FindLowestReq(std::deque<Request>& ReqDeq);
    int     FindHighestReq(std::deque<Request>& ReqDeq);
    Worker* FindLowestPriorityWorker(bool isReqAbsolute);
public:
    TimeManager(int req_need,int WorkersCount, double _AverageServiceTime, double _AverageReqAdmissionTime, bool SetTime);
    ~TimeManager();
    double AverageServiceTime;
	double AverageReqAdmissionTime;
	int ReqAdded = 0;
	int ReqNeed = 100;
	int Limit=26;
	std::multiset<Event> TimeHandle;

    std::deque<Request> ReqFailed;
    ////GRAPHIC DATA///
    QVector<double> ExpRaspAdmission;
    QVector<double> ExpRaspService;

    QVector<double> ExpRaspDensityService;
    QVector<double> ExpRaspDensityAdmission;

    QVector<double> ReqCountPerFix;
    QVector<double> ReqCountInSystemPerFix;

    QVector<double> NsPerFix;
    QVector<double> NqPerFix;
    QVector<double> PPerFix;

    QVector<double> Time;

    QVector<double>* ExpTime;
    void SetGraphicsDataExpAdmission();
    void SetGraphicsDataExpService();

    void SetGraphicsDataExpDensityAdmission();
    void SetGraphicsDataExpDensityService();

    //////////////

    double TypeRequestBound=0.8;

    int CountOfWorkers;

    Worker* GroupOfWorkers;

    QMap<int,QPair<double,double>> LowPriorityArr;
    QMap<int,QPair<double,double>> HighPriorityArr;

	double CurrentTime = 0.f;

	void AddNextReqBeforeSomeTime();

	void RequestInWork();
	void CheckReqContainer();
	Event MoveTime();

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
    //������������� ���������� ����������� �������
    double GetAbsoluteSystemCapacityCr() const;

	//��������� ��������� �����
	double Exponential_rasp(double med) const;
    //true-������� ���������
    //false-������ ���������
    bool   GetTypeForRequest();
    int    GetPriorityForRequest(bool isRequestAbsolute);
    int    FindPriorityInMap(QMap<int, QPair<double,double>>& PriorityMap, double Priority);

	int	   GetReqCountInDeq() const;
    int    GetReqCountInSystem() const;

    static bool TimeEquivalently(double l, double r);
};

