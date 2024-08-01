//启动服务器:  g++ *.cpp   ./a.out
//启动客户端:  nc 127.0.0.1 8888 

#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream> 
#include <unistd.h>
using std::cout;
using std::endl;

void test(){
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready();  //此时处于监听状态

    EventLoop loop(acceptor);
    loop.loop();
}

int main()
{
    test();
    return 0;
}
