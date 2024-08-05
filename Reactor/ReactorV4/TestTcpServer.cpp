//启动服务器:  g++ *.cpp   ./a.out
//启动客户端:  nc 127.0.0.1 8888 

#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include "TcpServer.h"
#include <iostream> 
#include <unistd.h>
using std::cout;
using std::endl;
using TcpConnectionPtr = shared_ptr<TcpConnection>;                           
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>; 

ThreadPool* gPool = nullptr;

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {

    }

    void process()
    {
        //可以在process函数中进行业务逻辑的处理
        _msg = "hello baby\n";
        _con->sendInLoop(_msg);
    }

private:
    string _msg;
    TcpConnectionPtr _con;
};

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
    cout << ">>recv msg from client: " << msg;
    
    //现在业务逻辑的处理，交由线程池进行
    MyTask task(msg, con); 
    gPool->addTask(std::bind(&MyTask::process, task));
}

//连接断开
void onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << "has closed!!!" << endl;
}


//V4版本:加入了线程池
void test4()
{
    //启动线程池
    ThreadPool pool(4, 10);
    pool.start();    //线程池中的线程阻塞等待任务的到来
    gPool = &pool;

    TcpServer server("127.0.0.1", 8888);
    //注册回调
    server.setAllCallback(std::move(onNewConnection), std::move(OnMessage), std::move(onClose));
    server.start();
}

int main()
{
    test4();
    return 0;
}
