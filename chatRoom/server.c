#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#define ACCEPT_QUEUE_SIZE  5
#define PORT 13666

#define MAX_MSG_LEN 1024
#define MAX_CLIENTS  100
#define MAX_NAME_LEN 32

void fillAddress(struct sockaddr_in *addr, uint16_t port);

void doBind(int sd, struct sockaddr_in addr);

void send_message_other(const char *s, int fd);

void *handleClient(void *arg);

void doListen(int sd);

int openServerSocket(uint16_t port);

int doAccept(int sd);

int serverSocket();

static const char apologise[] = "Sorry, there are too many clients\n";
static const char enter_name[] = "Please, introduce yourself\n";

typedef struct
{
    int connect_fd;
    ssize_t name_len;
    char name[MAX_NAME_LEN];
} client_t;

client_t *clients[MAX_CLIENTS];
unsigned int client_counter = 0;


int main()
{
    int sd = openServerSocket(PORT);

    while (1)
    {
        // descriptor of new client
        int cliSd = doAccept(sd);

        if ((client_counter + 1) == MAX_CLIENTS)
        {
            write(cliSd, apologise, sizeof(apologise));
            close(cliSd);
            exit(EXIT_FAILURE);
        }

        client_t *client = (client_t *) calloc(1, sizeof(client_t));
        client->connect_fd = cliSd;

        // create thread for new client
        pthread_t cli_pid;
        pthread_create(&cli_pid, NULL, &handleClient, (void *) client);
    }
}


void add_client(client_t *client)
{
    assert(client);

    int i = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (!clients[i])
        {
            clients[i] = client;
            return;
        }
    }
}


void delete_client(int client_fd)
{
    int i = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i])
        {
            if (clients[i]->connect_fd == client_fd)
            {
                close(clients[i]->connect_fd);
                free(clients[i]);

                clients[i] = NULL;
                client_counter--;
                return;
            }
        }
    }
}


void send_message_other(const char *s, int fd)
{
    assert(s);

    int i = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i] && clients[i]->connect_fd != fd)
        {
            if (write(clients[i]->connect_fd, s, strlen(s)) < 0)
            {
                printf("client %d doesn't response\n", clients[i]->connect_fd);
                delete_client(clients[i]->connect_fd);
            }
        }
    }
}


void *handleClient(void *arg)
{
    assert(arg);

    client_t *cli = (client_t *) arg;
    char buff_in[MAX_MSG_LEN + MAX_NAME_LEN];

    //Read client's name
    if (write(cli->connect_fd, enter_name, sizeof(enter_name)) < 0)
    {
        printf("client %d doesn't response\n", cli->connect_fd);
        delete_client(cli->connect_fd);
        return 0;
    }
    if ((cli->name_len = read(cli->connect_fd, buff_in, MAX_NAME_LEN)) < 0)
    {
        printf("client %d doesn't response\n", cli->connect_fd);
        delete_client(cli->connect_fd);
        return 0;
    }

    add_client(cli);
    printf("add client %d\n", cli->connect_fd);

    // some kluges for decoration:)
    buff_in[cli->name_len - 1] = ' ';
    buff_in[cli->name_len - 2] = ':';

    while (read(cli->connect_fd, buff_in + cli->name_len, MAX_MSG_LEN - 2) > 0)
    {
        printf("client %d send message\n", cli->connect_fd);
        send_message_other(buff_in, cli->connect_fd);

    }

    printf("delete client %d\n", cli->connect_fd);
    delete_client(cli->connect_fd);
}


int doAccept(int sd)
{
    struct sockaddr *clientAddress = NULL;
    int cliSd = accept(sd, clientAddress, NULL);
    if (cliSd == -1)
    {
        perror("accept() fail");
        exit(EXIT_FAILURE);
    }
    return cliSd;
}


int openServerSocket(uint16_t port)
{
    struct sockaddr_in address;
    fillAddress(&address, port);

    int sd = serverSocket();
    doBind(sd, address);
    doListen(sd);
    return sd;
}


void doListen(int sd)
{
    if (listen(sd, ACCEPT_QUEUE_SIZE))
    {
        perror("listen() fail");
        exit(EXIT_FAILURE);
    }
}


int serverSocket()
{
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0)
    {
        perror("socket() fail");
        exit(EXIT_FAILURE);
    }
    return sd;
}


void doBind(int sd, struct sockaddr_in addr)
{
    if (bind(sd, (const struct sockaddr *) &addr, sizeof(addr)))
    {
        perror("bind() fail");
        close(sd);
        exit(EXIT_FAILURE);
    }
}


void fillAddress(struct sockaddr_in *addr, uint16_t port)
{
    bzero(addr, sizeof(*addr));
    (*addr).sin_family = AF_INET;
    (*addr).sin_addr.s_addr = INADDR_ANY;
    (*addr).sin_port = htons(port);
}
