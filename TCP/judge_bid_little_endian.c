//判断自己的电脑是大端方式还是小端方式

#include <stdio.h>

int main() {
    int num = 0x12345678;
    char* p = (char*)&num;
    if(*p == 0x12){
        printf("大端方式\n");
    }else if(*p == 0x78){
        printf("小端方式\n");
    }else{
        printf("未知字节序\n");
    }
    printf("num = %x\n", num);
    printf("num in memory:%x %x %x %x\n,",*p,*(p+1),*(p+2),*(p+3));
    return 0;
}
