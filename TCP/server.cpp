//服务器端代码
//一次发送,一次接收

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string>
using std::cout;
using std::cin;
using std::cerr;
using std::endl;
using std::string;

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cerr << "用法: ./server IP port" << endl;
        cout << "请重新输入" << endl;
        return 1;
    }

    //1、需要创建套接字，使用socket函数
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        perror("socket");
        return -1;
    }

    //设置端口复用与地址复用
    int opt = 1;
    int retval = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(retval < 0)
    {
        perror("setsockopt addr");
        close(listenfd);
        return -1;
    }

    int opt2 = 1;
    retval = setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt2, sizeof(opt2));
    if(retval < 0)
    {
        perror("setsockopt port");
        close(listenfd);
        return -1;
    }

    //2、需要绑定服务器的ip与端口号，使用bind函数
    //对应会有结构体struct sockadd_in
    struct sockaddr_in seraddr;
    memset(&seraddr, 0, sizeof(seraddr));//初始化seraddr
    seraddr.sin_family = AF_INET;

    //本机字节序转换为网络字节序(包括ip与端口号)
    seraddr.sin_addr.s_addr = inet_addr(argv[1]);
    seraddr.sin_port = htons(atoi(argv[2]));

    int ret = bind(listenfd, (struct sockaddr *)&seraddr,
                   sizeof(seraddr));
    if(ret < 0)
    {
        perror("bind");
        close(listenfd);
        return -1;
    }

    //3、服务器会处于监听状态，使用listen函数
    ret = listen(listenfd, 128);
    if(ret < 0)
    {
        perror("listen");
        close(listenfd);
        return -1;
    }

    cout << "server starts listening..." << endl;

    //4、服务器会调用accept函数，阻塞等待客户端的连接
#if 0
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));//初始化seraddr
    seraddr.sin_family = AF_INET;
    socklen_t length = sizeof(cliaddr);
    accept(listenfd, (struct sockaddr *)&cliaddr, &length);
#endif
    int connfd = accept(listenfd, nullptr, nullptr);
    if(connfd < 0)
    {
        perror("accept");
        close(listenfd);
        return -1;
    }

    //5、连接成功之后，表明三次握手建立成功，就可以
    //进行数据的收发，也就是调用read/recv,或者write/send
    while(1)
    {
        char buf[128] = {0};
        int len = recv(connfd, buf, sizeof(buf), 0);
        if(len < 0)
        {
            cout << "服务器接收数据失败" << endl;
        }
        else if(0 == len)
        {
            cout << "len == 0" << endl;
        }
        else
        {
            cout << "recv msg from client : " << buf << endl;
        }

        cout << "我服务器想发送数据给客户端: ";
        string line;
        getline(cin, line);
        int len2 = send(connfd, line.data(), line.size(), 0);
        if(len2 < 0)
        {
            cout << "服务器发送出错" << endl;
        }
        else if(0 == len2)
        {
            cout << "0 == len2" << endl;
        }
        else
        {
            cout << "服务器发送正常。等待客户端回消息...\n";
        }
    }

    //6、关闭对应的文件描述符
    close(listenfd);
    close(connfd);
    return 0;
}
