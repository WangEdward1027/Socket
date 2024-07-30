#include "TcpConnection.h"
#include <sstream>
using std::ostringstream;

TcpConnection::TcpConnection(int fd)
: _sockIO(fd)
, _sock(fd)
, _localAddr(getLocalAddr())
, _peerAddr(getPeerAddr())
{

}

TcpConnection::~TcpConnection()
{

}

string TcpConnection::receive()
{
    char buff[65535] = {0};
    _sockIO.readLine(buff, sizeof(buff));
    return string(buff);
}

void TcpConnection::send(const string & msg)
{
    _sockIO.writen(msg.c_str(), msg.size());
}

string TcpConnection::toString()
{
    ostringstream oss;
    oss << _localAddr.ip() << ":"
        << _localAddr.port() << "---->"
        << _peerAddr.ip() << ":"
        << _peerAddr.port();
    return oss.str();
}

//获取本端的网络地址信息
InetAddress TcpConnection::getLocalAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    //获取本端的ip地址：getsockname
    int ret = getsockname(_sock.getFd(), (struct sockaddr *)&addr, &len);
    if(ret == -1){
        perror("getsockname");
    }
    return InetAddress(addr);
}

//获取对端的网络地址信息
InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    //获取对端地址的函数:getpeername
    int ret = getpeername(_sock.getFd(), (struct sockaddr *)&addr, &len);
    if(ret == -1){
        perror("getpeername");
    }
    return InetAddress(addr);
}
