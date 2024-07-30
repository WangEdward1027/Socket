#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"

class TcpConnection
{
public:
    explicit TcpConnection(int fd);
    ~TcpConnection();
    string receive();
    void send(const string & msg);
    string toString();
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

private:
    SocketIO _sockIO;
    Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;
};

#endif

