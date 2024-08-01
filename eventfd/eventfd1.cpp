//eventfd用于多线程编程:生产者消费者模型

#include <sys/eventfd.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

// 创建eventfd
int efd = eventfd(0, 0);

void* producer(void* arg) {
    uint64_t u = 1;
    for (int i = 0; i < 5; ++i) {
        sleep(1); // 模拟工作
        write(efd, &u, sizeof(uint64_t)); // 通知消费者
        printf("Produced event %d\n", i + 1);
    }
    return NULL;
}

void* consumer(void* arg) {
    uint64_t u;
    for (int i = 0; i < 5; ++i) {
        read(efd, &u, sizeof(uint64_t)); // 等待事件
        printf("Consumed event %d\n", i + 1);
    }
    return NULL;
}

int main() {
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    close(efd);
    return 0;
}
