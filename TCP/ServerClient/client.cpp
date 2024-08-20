//客户端代码
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
using std::endl;
using std::cerr;
using std::string;

//argc 命令行参数的个数
//argv命令行数据对应的数组
int main(int argc, char **argv)
{
    if(argc != 3)
    {
        cerr << "用法: ./client IP port" << endl;
        cout << "请重新输入" << endl;
        return 1;
    }

    //1、创建套接字，使用socket函数
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
        perror("socket");
        return -1;
    }
    //2、主动发起连接请求，使用connect函数，但是需要知道
    //服务器的ip与端口号port，使用struct sockadd_in结构体
    struct sockaddr_in seraddr;
    memset(&seraddr, 0, sizeof(seraddr));//初始化seraddr
    seraddr.sin_family = AF_INET;
    //本机字节序转换为网络字节序(包括ip与端口号)
    seraddr.sin_addr.s_addr = inet_addr(argv[1]);
    seraddr.sin_port = htons(atoi(argv[2]));
    
    //3.连接服务器
    int ret = connect(listenfd, (struct sockaddr *)&seraddr, sizeof(seraddr));
    if(ret < 0)
    {
        perror("connect");
        close(listenfd);
        return -1;
    }

    //4.收发数据
    //如果connect返回值是ok，那么就表明三次握手建立成功了
    //那么就可以进行数据的收发，也就是read/recv，或者write/send函数
    while(1)
    {
        //读取用户输入
        cout << "我客户端想发送数据给服务器: ";
        string line;
        getline(cin, line);

        //发送数据到服务器
        int len2 = send(listenfd, line.data(), line.size(), 0);
        if(len2 < 0){
            cout << "客户端发送出错" << endl;
        }else if(0 == len2){
            cout << "0 == len2" << endl;
        }else{
            cout << "客户端发送正常，等待服务器回消息...\n";
        }
        
        //读取服务器返回的数据
        char buf[128] = { };
        int len = recv(listenfd, buf, sizeof(buf), 0);
        if(len < 0){
            cout << "客户端接收失败" << endl;
        }else if(0 == len){
            cout << "len == 0" << endl;
        }else{
            cout << "recv msg from server: " << buf << endl;
        }
    }

    //5.关闭文件描述符
    close(listenfd);
    return 0;
}
