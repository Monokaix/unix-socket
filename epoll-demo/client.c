//
// Created by c00577049 on 2022/7/27.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LINE (3)
#define SERVER_PORT (8088)

void set_non_blocking(int fd) {
    int opts = 0;
    opts = fcntl(fd, F_GETFL);
    opts = opts | O_NONBLOCK;
    fcntl(fd, F_SETFL);
}

int main(int argc, char **argv) {
    int sockfd;
    char recvline[MAX_LINE + 1] = {0};

    struct sockaddr_in server_addr;

    if (argc != 2) {
        fprintf(stderr, "usage ./client <SERVER_IP>\n");
        exit(0);
    }

    // 创建socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error");
        exit(0);
    }

    // 给server addr赋值
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s", argv[1]);
        exit(0);
    }

    // 连接服务端
    if (connect(sockfd, (struct sockaddr *) (&server_addr), sizeof(server_addr)) < 0) {
        perror("connect");
        fprintf(stderr, "connect error\n");
        exit(0);
    }

    set_non_blocking(sockfd);

    char input[100];
    int n = 0;
    int count = 0;


    // 循环从标准输入读取数据
    while (fgets(input, 100, stdin) != NULL) {
        printf("[send] %s\n", input);
        n = 0;
        // 读取的数据发送到服务器
        n = send(sockfd, input, strlen(input), 0);
        if (n < 0) {
            perror("send");
        }

        n = 0;
        count = 0;

        // 读取服务器返回的数据
        while (1) {
            n = read(sockfd, recvline + count, MAX_LINE);
            printf("n====%d\n", n);
            if (n == MAX_LINE) {
                count += n;
                continue;
            } else if (n < 0) {
                perror("read");
                break;
            } else {
                count += n;
                recvline[count] = '\0';
                printf("[recv] %s\n", recvline);
                break;
            }
        }
    }

    return 0;
}