#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/epoll.h>

#define SERVER_PORT 8088
#define EPOLL_MAX_NUM 2048
#define BUFFER_MAX_LENGTH 4096

char buffer[BUFFER_MAX_LENGTH];

void str_toupper(char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
}

int main(int argc, char **argv) {
    int listen_fd, client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;

    int epfd = 0;
    struct epoll_event event, *my_events;

    // socket: 返回一个套接字
    // af: 协议族。AF_INET: 表示IPV4
    // type: socket类型。SOCK_STREAM: 连接类型是双向流, 即TCP
    // protocol: 协议,TCP或UDP等, 等于0时表示根据type字段自动选择对应的协议
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // bind: 将描述符和监听地址绑定
    server_addr.sin_family = AF_INET;
    // htonl: 主机序转换为网络序, 小端转换为大端
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);
    bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

    // listen
    listen(listen_fd, 10);

    // create epoll
    epfd = epoll_create(EPOLL_MAX_NUM);
    if (epfd < 0) {
        perror("epoll create");
        goto END;
    }

    // 注册listen_fd到epoll
    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &event) < 0) {
        perror("epoll ctl add listen_fd ");
        goto END;
    }
    printf("successfully epoll_ctl listen fd\n");

    // 接收的事件
    my_events = malloc(sizeof(struct epoll_event) * EPOLL_MAX_NUM);

    // 循环接收事件
    while (1) {
        printf("begin epoll wait...\n");
        // 调用epoll_wait等待 -1表示无限阻塞
        int active_fds_cnt = epoll_wait(epfd, my_events, EPOLL_MAX_NUM, -1);
        for (int i = 0; i < active_fds_cnt; i++) {
            // 监听fd有事件, 说明有新的连接进来, 调用accept接收处理
            if (my_events[i].data.fd == listen_fd) {
                // 后面两个参数用来获取client的ip和端口信息
                client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                printf("new connection comes\n");
                printf("IP address is: %s\n", inet_ntoa(client_addr.sin_addr));
                printf("port is: %d\n", (int) ntohs(client_addr.sin_port));

                // 注册客户端的fd到epoll
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
            } else if (my_events[i].events & EPOLLIN) {
                printf("EPOLLIN\n");
                client_fd = my_events[i].data.fd;

                // do read
                buffer[0] = '\0';
                // 固定读取最大字节数为5
                int n = read(client_fd, buffer, 5);
                if (n < 0) {
                    perror("read");
                    continue;
                } else if (n == 0) { // 连接已关闭
                    printf("close...\n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, &event);
                    close(client_fd);
                } else {
                    printf("[read]: %s\n", buffer);
                    buffer[n] = '\0';
                    str_toupper(buffer);
                    write(client_fd, buffer, strlen(buffer));
                    printf("[write]: %s \n", buffer);
                    memset(buffer, 0, BUFFER_MAX_LENGTH);
                }
            } else if (my_events[i].events & EPOLLOUT) {
                printf("EPOLLOUT\n");
            }
        }
    }

    END:
    close(epfd);
    close(listen_fd);
    return 0;
}
