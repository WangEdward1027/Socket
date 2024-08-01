//eventfd用于网络编程：配合epoll
//在网络编程中，eventfd 常用于事件驱动模型，尤其是基于 epoll 的模型中。它可以帮助实现以下功能：
//1.事件通知：在 epoll 中，eventfd 可以用来通知主线程有新的网络事件需要处理。
//2.异步 I/O：配合 epoll 等异步 I/O 机制，eventfd 可以用来管理和分发事件，避免阻塞。

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

int efd;
int epoll_fd;

void* worker_thread(void* arg) {
    uint64_t u = 1;
    while (1) {
        sleep(2); // 模拟工作
        write(efd, &u, sizeof(uint64_t)); // 通知主线程
    }
    return NULL;
}

void handle_eventfd() {
    uint64_t u;
    read(efd, &u, sizeof(uint64_t));
    printf("Handled event\n");
}

int main() {
    efd = eventfd(0, 0);
    epoll_fd = epoll_create1(0);

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = efd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, efd, &event);

    pthread_t worker;
    pthread_create(&worker, NULL, worker_thread, NULL);

    while (1) {
        struct epoll_event events[10];
        int nfds = epoll_wait(epoll_fd, events, 10, -1);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == efd) {
                handle_eventfd();
            }
        }
    }

    close(epoll_fd);
    close(efd);
    return 0;
}
