#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>
using std::vector;
using std::map;
using std::shared_ptr;
using std::function;

class Acceptor; //前向声明
class TcpConnection;

class EventLoop
{
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;  
public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();
    
    //是否在循环
    void loop();
    void unloop();
   
    //初始化三个TcpConnection数据成员
    void setNewConnectionCallback(TcpConnectionCallback &&);
    void setMessageCallback(TcpConnectionCallback &&);
    void setCloseCallback(TcpConnectionCallback &&);
private:
    //封装epoll_wait
    void waitEpollFd();

    //处理新的连接
    void handleNewConnection();

    //处理老连接上的数据收发
    void handleMessage(int fd);

    //封装epoll_create函数
    int createEpollFd();

    //将文件描述符放在红黑树上进行监听
    void addEpollReadFd(int fd);
    
    //将文件描述符从红黑树上取消监听
    void delEpollReadFd(int fd);

private:
    int _epfd;  //由epoll_create创建的文件描述符
    vector<struct epoll_event> _evtList; //交给epoll_wait的第二个参数,存放就绪的文件描述符
    bool _isLooping; //标识循环是否在运行
    Acceptor &_acceptor; //获取Acceptor类型的引用,目的是为了调用其中的accept函数
    map<int, shared_ptr<TcpConnection>> _conns; //存放文件描述符与TcpConnection键值对
 
    //回调的数据成员,function对象: function<void(const shared_ptr<TcoConnection> &)> _onxxx
    TcpConnectionCallback _onNewConnectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;
};

#endif
