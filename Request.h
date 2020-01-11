#pragma once
class Request
{
public:
    Request(double work_time,int _req_priority, double _work_begin_time, double arrival_time, double exit_time);
	Request(double work_time,double _work_begin_time, double arrival_time, double exit_time);
	Request(double work_time, double arrival_time, double exit_time);
	Request(double work_time, double arrival_time);
	Request();
    int    ReqPriority;
	double WorkTime;
	double ArrivalTime;
	double WorkBeginTime;
	double ExitTime;
};

