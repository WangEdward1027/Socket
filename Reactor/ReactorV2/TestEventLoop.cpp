//启动服务器:  g++ *.cpp   ./a.out
//启动客户端:  nc 127.0.0.1 8888 

#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include <iostream> 
#include <unistd.h>
using std::cout;
using std::endl;

void test0()
{
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready(); //处于监听状态

    //三次握手已建立,可以创建一条TCP连接
    TcpConnection con(acceptor.accept());

    //加调试,打印本端与对端的ip与端口号
    cout << con.toString() << "has connected" << endl;

    while(1){
        cout << ">>recv msg from client: " << con.receive() << endl;
        con.send("hello Edward\n");
    }
}

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
