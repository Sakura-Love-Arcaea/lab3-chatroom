#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "setup.c"
typedef struct sockaddr_in sockaddr_in;

int main() {
    int         sock_server;
    sockaddr_in server;
    char        sendBuf[256] = {0};
    char        readBuf[256] = {0};
    int         n;

    sock_server = setup(&server, "127.0.0.1", 8080);
    if (connect(sock_server, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        exit(0);
    }

    n = recv(sock_server, readBuf, sizeof(readBuf), 0);
    if (strcmp(readBuf, "Server is full") == 0) {
        printf("server is full\n");
        exit(0);
    } else if (strcmp(readBuf, "sussess") == 0) {
        printf("connect sussess\n");
    }
    memset(readBuf, 0, sizeof(readBuf));
    pid_t pid = fork();
    while (1) {
        if (pid < 0) {
        } else if (pid == 0) {
            // child to read
            n = recv(sock_server, readBuf, sizeof(readBuf), 0);
            if (n == 0) {
                puts("server disconnect");
                kill(pid, SIGTERM);
                break;
            }
            printf("\rchat: %s\n", readBuf);
            putchar('>');
            fflush(stdout);
            memset(readBuf, 0, sizeof(readBuf));
            // printf("RSock ID: %d\n", sock_server);
            // printf("Read message: %s\n\n", readBuf);
        } else {
            // parent to send
            putchar('>');
            if (scanf("%255[^\n]%*c", sendBuf) != EOF) {
                if (strlen(sendBuf) == 0) {
                    fflush(stdin);
                    continue;
                }
                send(sock_server, sendBuf, sizeof(sendBuf), 0);

                // printf("SSock ID: %d\n", sock_server);
                // printf("Send message: %s\n\n", sendBuf);
            } else {
                printf("\rbye\n");
                kill(pid, SIGTERM);
                break;
            }
            memset(sendBuf, 0, sizeof(sendBuf));
        }
    }
    close(sock_server);
    return 0;
}