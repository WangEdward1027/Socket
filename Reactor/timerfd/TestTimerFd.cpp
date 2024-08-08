#include "TimerFd.h"
#include <unistd.h>
#include <iostream>
#include <functional>
#include <thread>
using std::cout;
using std::endl;
using std::bind;
using std::thread;

class MyTask
{
public:
    void process()
    {
        cout << "MyTask is running!!!" << endl;
    }
};

int main()
{
    MyTask task;
    TimerFd tfd(bind(&MyTask::process, &task), 1, 3);
    
    thread th(bind(&TimerFd::start, &tfd)); //th是A线程,主线程是B线程
    
    sleep(19);

    tfd.stop();
    th.join();
    
    return 0;
}
