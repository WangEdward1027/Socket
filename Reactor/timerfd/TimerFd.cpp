#include "TimerFd.h"
#include <unistd.h>
#include <poll.h>
#include <sys/timerfd.h>
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

TimerFd::TimerFd(TimerFdCallback &&cb, int initsec, int peridocSec)
: _timerfd(createTimerFd())
, _cb(std::move(cb))
, _isStarted(false)
, _initSec(initsec)
, _peridocSec(peridocSec)
{

}

TimerFd::~TimerFd()
{
    close(_timerfd);
}

//运行与停止
void TimerFd::start()
{
    struct pollfd pfd;
    pfd.fd = _timerfd;
    pfd.events = POLLIN;

    //可以设置定时器
    setTimerFd(_initSec, _peridocSec);

    _isStarted = true;

    while(_isStarted){
        int nready = poll(&pfd, 1 ,5000);
        if(-1 == nready && errno == EINTR){
            continue;
        }else if(-1 == nready){
            cerr << "-1 == nready" << endl;
            return;
        }else if(0 == nready){
            cout << ">>poll timeout!!!" << endl;
        }else{
            if(pfd.revents & POLLIN){
                handleRead();
                if(_cb){
                    _cb();   //被唤醒后要执行的任务
                }
            }
        }
    }

}

void TimerFd::stop()
{
    if(_isStarted){
        _isStarted = false;
        setTimerFd(0, 0);
    }
}

//创建用于通信的文件描述符
int TimerFd::createTimerFd()
{
    int fd = timerfd_create(CLOCK_REALTIME, 0);
    if(fd < 0){
        perror("createTimerFd");
        return -1;
    }
    return fd;
}

//封装read
void TimerFd::handleRead()
{
    uint64_t one = 1;
    ssize_t ret = read(_timerfd, &one, sizeof(uint64_t));
    if(ret != sizeof(uint64_t)){
        perror("handleRead");
        return;
    }
}

//封装timerfd_settime
void TimerFd::setTimerFd(int initSec, int peridocSec)
{
    struct itimerspec newValue;
    newValue.it_value.tv_sec = initSec; //起始时间
    newValue.it_value.tv_nsec = 0;

    newValue.it_interval.tv_sec = peridocSec; //周期时间
    newValue.it_interval.tv_nsec = 0;

    int ret = timerfd_settime(_timerfd, 0 ,& newValue, nullptr);
    if(ret < 0){
        perror("setTimeFd");
        return;
    }
}
