//nc命令等价的客户端(c语言实现)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int str_len;

    //1.创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() error");
        exit(1);
    }

    //2.初始化服务器地址结构体
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 服务器IP地址
    server_addr.sin_port = htons(8888); // 服务器端口号

    //3.连接服务器
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() error");
        close(sock);
        exit(1);
    }

    //4.数据传输:发送和接收数据
    while (1) {
        // 读取标准输入的数据
        printf("Input: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // 发送数据到服务器
        write(sock, buffer, strlen(buffer));

        // 读取服务器返回的数据
        str_len = read(sock, buffer, BUFFER_SIZE - 1);
        if (str_len == -1) {
            perror("read() error");
            break;
        }

        buffer[str_len] = '\0';
        printf("Message from server: %s\n", buffer);
    }

    //5.关闭套接字
    close(sock);
    return 0;
}
