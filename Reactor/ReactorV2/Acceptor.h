#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Socket.h"
#include "InetAddress.h"

class Acceptor
{
public:
    Acceptor(const string & ip, unsigned short port);
    ~Acceptor();
    void ready();
    void setReuseAddr();
    void setReusePort();
    void bind();
    void listen();
    int accept();
    int getfd() const;

private:
    Socket _sock;
    InetAddress _addr;
};

#endif

