extern int sockList[256];
extern int queue[256];
extern int working;

void boradcast(char *buf, int self) {
    for (int i = 0; i < 256; i++) {
        if (sockList[i] != 0 && sockList[i] != self) {
            send(sockList[i], buf, strlen(buf), 0);
        }
    }
}

int queryOrder(int sock) {
    for (int i = 0; i < 255; i++) {
        if (sockList[i] == sock) {
            return i;
        }
    }
    return -1;
}

void *connection_handler(void *sock) {
    int  sock_client = *(int *)sock;
    int  n;
    int  index = working - 1;
    char buf[256];
    printf("---connected(socket): %d\n---total: %d\n", sock_client, working);
    if (sockList[index] == 0) {
        sockList[index] = sock_client;
    }
    sprintf(buf, "socket(%d) is join", sock_client);
    boradcast(buf, sock_client);
    while (1) {
        n = recv(sock_client, buf, sizeof(buf), 0);
        if (n > 0) {
            if (buf[0] == '/') {
                // command
                if (strcmp(buf, "/room") == 0) {
                    printf("command from socket(%d): %s\n", sock_client, buf);
                    sprintf(buf, "there are %d client in chat room", working);
                    send(sock_client, buf, sizeof(buf), 0);
                } else if (strcmp(buf, "/order") == 0) {
                    printf("command from socket(%d): %s\n", sock_client, buf);
                    printf("socket(%d) order is %d\n", sock_client, queryOrder(sock_client));
                    sprintf(buf, "you are no.%d client", queryOrder(sock_client) + 1);
                    send(sock_client, buf, sizeof(buf), 0);
                }
            } else {
                printf("message from socket(%d): %s\n", sock_client, buf);
                // boradcast
                boradcast(buf, sock_client);
            }
        } else if (n == 0) {
            working--;
            printf("client(%d) disconnect\n", sock_client);
            // memset(buf, 0, sizeof(buf));
            sprintf(buf, "socket(%d) was leave", sock_client);
            boradcast(buf, sock_client);
            close(sock_client);
            pthread_exit(0);
        }
    }
    return 0;
}