//启动服务器:  g++ *.cpp   ./a.out
//启动客户端:  nc 127.0.0.1 8888 

#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include <iostream> 
#include <unistd.h>
using std::cout;
using std::endl;

 using TcpConnectionPtr = shared_ptr<TcpConnection>;                           
 using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>; 

//连接建立
void onNewConnection(const TcpConnectionPtr &con)
{
    cout << con->toString() << "has connected!!!" << endl;
}

//消息到达
void OnMessage(const TcpConnectionPtr &con)
{
    //接收客户端发来的数据
    string msg = con->receive();
    cout << ">>recv msg from client: " << msg << endl;

    //做业务逻辑的处理
    msg = "hello Edward\n";

    //业务逻辑处理完毕后,将数据msg发给客户端
    con->send(msg);
}

//连接断开
void onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << "has closed!!!" << endl;
}

void test1()
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

//V2版本:TestEventLoop
void test2(){
    Acceptor acceptor("127.0.0.1", 8888);
    acceptor.ready();  //此时处于监听状态
    EventLoop loop(acceptor);

    loop.setNewConnectionCallback(std::move(onNewConnection));
    loop.setMessageCallback(std::move(OnMessage));
    loop.setCloseCallback(std::move(onClose));
    
    loop.loop();
}

//V3版本:TestTcpServer
void test3()
{
    TcpServer server("127.0.0.1", 8888);
    //注册回调
    server.setAllCallback(std::move(onNewConnection), std::move(OnMessage), std::move(onClose));
    server.start();
}

int main()
{
    test3();
    return 0;
}
