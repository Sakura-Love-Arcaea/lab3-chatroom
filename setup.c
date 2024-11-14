#include <netinet/in.h>
typedef struct sockaddr_in sockaddr_in;

int setup(struct sockaddr_in *sockaddr, char *ip, int port) {
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(sockaddr, 0, sizeof(sockaddr_in));
    sockaddr->sin_family      = PF_INET;
    sockaddr->sin_addr.s_addr = inet_addr(ip);
    sockaddr->sin_port        = htons(port);
    return sock;
}