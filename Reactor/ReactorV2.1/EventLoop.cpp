#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <utility> //std::move()
using std::cerr;
using std::cout;
using std::endl;
using std::string;

EventLoop::EventLoop(Acceptor &acceptor)
: _epfd(createEpollFd())
, _evtList(1024)
, _isLooping(false)
, _acceptor(acceptor) //引用数据成员,必须在构造函数的初始化列表中进行初始化
{
    //需要将listenfd放在红黑树上进行监听
    int listenfd = acceptor.getfd();
    addEpollReadFd(listenfd);
}

EventLoop::~EventLoop()
{
    close(_epfd);
}

//是否在循环
void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping){
        waitEpollFd();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

//封装epoll_wait
void EventLoop::waitEpollFd()
{   
    //获取vector第一个元素的首地址
    int nready = 0;
    do{
        nready = ::epoll_wait(_epfd, &_evtList[0], _evtList.size(), 3000);
    }while(-1 == nready && errno == EINTR);
    
    if(-1 == nready){
        cerr << "" << endl;
        return;
    }else if(0 == nready){
        cout << ">>epoll_wait timeout!!" << endl;
    }else{
        //为了防止文件描述符的数目超过1024,可以手动扩容
        if(nready == (int)_evtList.size()){
            _evtList.resize(2 * nready);
        }

        for(int idx = 0; idx < nready; ++idx){
            int fd = _evtList[idx].data.fd;
            int listenfd = _acceptor.getfd();

            if(fd == listenfd){
                handleNewConnection(); //处理新的连接请求
            }else{
                handleMessage(fd); //处理老的连接
            }
        }
    }
}

//处理新连接
void EventLoop::handleNewConnection()
{
    //如果connfd是正常的,表明三次握手成功
    int connfd = _acceptor.accept();
    if(connfd < 0){
        perror("handleNewConnection");
        return;
    }

    //将文件描述符connfd放在红黑树上进行监听
    addEpollReadFd(connfd);
    
    //三次握手成功,创建TcpConnection连接
    //用connfd创建TcpConnection对象,并交由智能指针con进行管理
    TcpConnectionPtr con(new TcpConnection(connfd));
 
    //可以在此处将EventLoop中的三个数据成员传递给TcpConnection对象
    con->setNewConnectionCallback(_onNewConnectionCb);
    con->setMessageCallback(_onMessageCb);
    con->setCloseCallback(_onCloseCb);

    //存储文件描述符与连接TcpConnection键值对
    /* _conns.insert({connfd,con}); */
    _conns[connfd] = con;

    //连接已经建立了,就可以执行连接建立的事件
    con->handleNewConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    auto it = _conns.find(fd);
    if(it != _conns.end())
    {
        bool flag = it->second->isClosed();  //TcpConnection的成员函数
        if(flag) //连接已经断开
        {
            it->second->handleCloseCallback(); //处理连接断开的事件
            delEpollReadFd(fd); //连接断开需要将文件描述符从红黑树上摘除
            _conns.erase(it); //再将文件描述符与连接的键值对从map中删除
        }else{
            it->second->handleMessageCallback(); //连接未断开,处理消息
        }
        /* //接收客户端的数据 */
        /* string msg = it->second->receive(); */
        /* cout << ">>recv msg from client: " << msg << endl; */
        
        /* //业务逻辑的处理 */
        /* msg = "hello Edward\n"; */

        /* //消息发送 */
        /* it->second->send(msg); */
    }else{
        cout << "连接不存在" << endl;
        return;
    }
}

//封装epoll_create函数
int EventLoop::createEpollFd()
{
    int fd = ::epoll_create(1);
    if(fd < 0){
        perror("createEpollFd");
        return -1;
    }
    return fd;
}

//将文件描述符放在红黑树上进行监听
void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt);
    if(ret < 0){
        perror("addEpoolReadFd");
        return;
    }
}

//将文件描述符从红黑树上取消监听
void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt);
    if(ret < 0){
        perror("delEpoolReadFd");
        return;
    }
}

void EventLoop::setNewConnectionCallback(TcpConnectionCallback &&cb)
{
    _onNewConnectionCb = std::move(cb); 
}

void EventLoop::setMessageCallback(TcpConnectionCallback &&cb)
{
    _onMessageCb = std::move(cb);
}

void EventLoop::setCloseCallback(TcpConnectionCallback &&cb)
{
    _onCloseCb = std::move(cb);
}
