#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <arpa/inet.h>

#include "netUtils.h"

#define MAX_IP_LEN 16
#define MAX_MSG_LEN 8192
#define SRV_PORT  13666

void fill_server_addr(const char *ip, struct sockaddr_in *servAddr);

void *get_msg(void *sock);

void get_ip(char *ip);


int main()
{
    char ip[MAX_IP_LEN] = {};
    struct sockaddr_in servAddr = {};
    char msg[MAX_MSG_LEN] = {};

    get_ip(ip);
    fill_server_addr(ip, &servAddr);

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
    pthread_create(&receive_thread, NULL, get_msg, &sd);
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


void *get_msg(void *sock)
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


void get_ip(char *ip)
{
    assert(ip);
    printf("Enter IP address:\n");
    fgets(ip, MAX_IP_LEN, stdin);

}


void fill_server_addr(const char *ip, struct sockaddr_in *servAddr)
{
    assert(ip);
    assert(servAddr);

    bzero(servAddr, sizeof(*servAddr));
    (*servAddr).sin_family = AF_INET;
    (*servAddr).sin_addr.s_addr = inet_addr(ip);
    (*servAddr).sin_port = htons(SRV_PORT);
}