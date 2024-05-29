#include <func.h>

//大端序、小端序

void test0(){
    int num = 0x12345678;
    char* p = (char*)&num;      //小端存储:主机字节序
    printf("首字节:%x\n",*p);   //78 56 34 12
    
    printf("\n");
}
    

void test1(){
    int num = 0x12345678;
    int netNum = htonl(num); //主机字节序->网络字节序
    
    //打印 num 和 netNum 的实际值
    printf("num:%x\n",num);  
    printf("netNum:%x\n",netNum);

    //打印num在内存中的字节顺序
    char* p = (char*)&num;
    printf("num in memory:%x %x %x %x\n",*p,*(p+1),*(p+2),*(p+3));

    //打印netNum在内存中的字节顺序
    char* p2 = (char*)&netNum; 
    printf("netNum in memory:%x %x %x %x\n",*p2,*(p2+1),*(p2+2),*(p2+3));

    printf("\n");
}

void test2(){
    unsigned short port = 8080;
    //把主机字节序转换为网络字节序
    unsigned short netPort = htons(port);
    printf("port:%x\n",port);
    printf("netPort:%x\n",netPort);

    //把网络字节序转换为主机字节序
    unsigned short port2 = ntohs(netPort);
    printf("port2:%x\n",port2);

    printf("\n");
}

int main(void)
{
    test0();
    test1();
    test2();
    return 0;
}
