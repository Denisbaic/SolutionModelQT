#include "Request.h"

Request::Request(double work_time, double _work_begin_time, double arrival_time, double exit_time): WorkTime(work_time),
ArrivalTime(arrival_time),WorkBeginTime(_work_begin_time),
ExitTime(exit_time)
{

}

Request::Request(double work_time, double arrival_time, double exit_time) : WorkTime(work_time),
                                                                            ArrivalTime(arrival_time), WorkBeginTime(0),
                                                                            ExitTime(exit_time)
{
}

Request::Request(double work_time, double arrival_time) : WorkTime(work_time),
                                                          ArrivalTime(arrival_time), WorkBeginTime(0),
                                                          ExitTime(0)
{
}

Request::Request() : WorkTime(0),
                     ArrivalTime(0), WorkBeginTime(0),
                     ExitTime(0)
{
}
