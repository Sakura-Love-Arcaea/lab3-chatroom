#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "connection_handler.c"
#include "setup.c"

#define MAX_CONNECT 2
typedef struct sockaddr_in sockaddr_in;

// variables
int         sock_server, sock_client;
sockaddr_in server, client;
socklen_t   client_len    = sizeof(sockaddr_in);
int         working       = 0;
int         sockList[256] = {0};
pthread_t   sniffer_thread;
int         queue[256] = {0};

void *time_interval(void *arg) {
    while (1) {
        sleep(1);
        printf("connections: %d\n", working);
    }
    return 0;
}

int main() {
    sock_server = setup(&server, "127.0.0.1", 8080);

    bind(sock_server, (struct sockaddr *)&server, sizeof(server));
    listen(sock_server, 5);

    printf("Server>\tReady\n");
    // pthread_create(&sniffer_thread, 0, time_interval, 0);
    // pthread_detach(sniffer_thread);

    while ((sock_client = accept(sock_server, (struct sockaddr *)&client, &client_len)) >= 0) {
        if (working < MAX_CONNECT) {
            send(sock_client, "sussess", sizeof("sussess"), 0);
            working++;
            pthread_create(&sniffer_thread, 0, connection_handler, (void *)&sock_client);
            pthread_detach(sniffer_thread);
        } else {
            send(sock_client, "Server is full", sizeof("Server is full"), 0);
            continue;
        }

        if (sock_client < 0) {
            printf("Csock ERROR");
            perror("Csock");
        }
    }
    close(sock_server);
    printf("[Socket Close]\n");
    return 0;
}