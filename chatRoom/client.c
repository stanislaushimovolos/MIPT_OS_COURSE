#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <assert.h>

#define MAX_MSG_LEN 1024
#define SRV_PORT  13666

static const char default_ip[] = "127.0.1";

void fillServerAddress(const char *ip, struct sockaddr_in *servAddr);

void *getMessage(void *sock);


int main()
{
    struct sockaddr_in servAddr = {};
    char msg[MAX_MSG_LEN] = {0};

    fillServerAddress(default_ip, &servAddr);
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket() fail");
        exit(EXIT_FAILURE);
    }
    if (connect(sd, (const struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    {
        perror("connect() fail");
        close(sd);
        exit(EXIT_FAILURE);
    }

    // Create thread for receiving
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, getMessage, &sd);
    while (1)
    {
        bzero(msg, MAX_MSG_LEN);
        fgets(msg, MAX_MSG_LEN, stdin);
        if (write(sd, msg, strlen(msg) + 1) < 0)
        {
            perror("write() fail");
            close(sd);
            exit(EXIT_FAILURE);
        }
    }
}


void *getMessage(void *sock)
{
    assert(sock);

    int socket = *((int *) sock);
    char msg[MAX_MSG_LEN] = {};
    ssize_t len = 0;

    while ((len = read(socket, msg, MAX_MSG_LEN)) > 0)
    {
        msg[len] = '\0';
        fputs(msg, stdout);
        bzero(msg, MAX_MSG_LEN);
    }

    // Check server
    printf("Server doesn't response\n");
    close(socket);
    exit(EXIT_FAILURE);
}


void fillServerAddress(const char *ip, struct sockaddr_in *servAddr)
{
    assert(ip);
    assert(servAddr);

    bzero(servAddr, sizeof(*servAddr));
    (*servAddr).sin_family = AF_INET;
    (*servAddr).sin_addr.s_addr = inet_addr(ip);
    (*servAddr).sin_port = htons(SRV_PORT);
}
