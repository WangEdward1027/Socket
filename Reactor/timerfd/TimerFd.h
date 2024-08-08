#ifndef __TIMERFD_H__
#define __TIMERFD_H__

#include <functional>
using std::function;

class TimerFd
{
    using TimerFdCallback = function<void()>;
public:
    TimerFd(TimerFdCallback &&cb, int initSec, int peridocSec);
    ~TimerFd();

    //运行与停止
    void start();
    void stop();

    //创建用于通信的文件描述符
    int createTimerFd();

    //封装read
    void handleRead();

    //封装timerfd_settime
    void setTimerFd(int initSec, int peridocSec);

private:
    int _timerfd; //用于超时通知的文件描述符
    TimerFdCallback _cb; //被超时通知后需要执行的任务
    bool _isStarted; //标识运行与否
    int _initSec; //定时器的起始时间
    int _peridocSec;  //定时器的超时时间
};

#endif
