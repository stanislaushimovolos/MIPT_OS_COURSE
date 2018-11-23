#include "netUtils.h"


void display_address()
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs()");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++)
    {

        family = ifa->ifa_addr->sa_family;

        if (family == AF_INET || family == AF_INET6)
        {
            s = getnameinfo(ifa->ifa_addr,
                            (family == AF_INET) ? sizeof(struct sockaddr_in) :
                            sizeof(struct sockaddr_in6),
                            host, NI_MAXHOST,
                            NULL, 0, NI_NUMERICHOST);
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("address: <%s>\n", host);
        }
    }
    freeifaddrs(ifaddr);
}


int do_accept(int sd)
{
    struct sockaddr *client_address = NULL;
    int cliSd = accept(sd, client_address, NULL);
    if (cliSd == -1)
    {
        perror("accept() fail");
        exit(EXIT_FAILURE);
    }
    return cliSd;
}


int open_server_sock(uint16_t port)
{
    struct sockaddr_in address;
    fill_address(&address, port);

    int sd = server_sock();
    do_bind(sd, address);
    do_listen(sd);
    return sd;
}


void do_listen(int sd)
{
    if (listen(sd, ACCEPT_QUEUE_SIZE))
    {
        perror("listen() fail");
        exit(EXIT_FAILURE);
    }
}


int server_sock()
{
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket() fail");
        exit(EXIT_FAILURE);
    }
    return sd;
}


void do_bind(int sd, struct sockaddr_in addr)
{
    if (bind(sd, (const struct sockaddr *) &addr, sizeof(addr)))
    {
        perror("bind() fail");
        close(sd);
        exit(EXIT_FAILURE);
    }
}


void fill_address(struct sockaddr_in *addr, uint16_t port)
{
    bzero(addr, sizeof(*addr));
    (*addr).sin_family = AF_INET;
    (*addr).sin_addr.s_addr = INADDR_ANY;
    (*addr).sin_port = htons(port);
}