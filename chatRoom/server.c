#include <stdio.h>
#include <pthread.h>
#include "netUtils.h"

#define PORT 13666

#define MAX_MSG_LEN 1024
#define MAX_CLIENTS  100
#define MAX_NAME_LEN 32

typedef struct
{
    int connect_fd;
    ssize_t name_len;
    char name[MAX_NAME_LEN];
} client_t;

void send_message_other(const char *s, int fd);

void *handle_client(void *arg);


static const char apologise[] = "Sorry, there are too many clients\n";
static const char enter_name[] = "Please, introduce yourself\n";

client_t *clients[MAX_CLIENTS];
unsigned int client_counter = 0;


int main()
{
    display_address();
    int sd = open_server_sock(PORT);

    while (1)
    {
        // descriptor of new client
        int cliSd = do_accept(sd);

        if ((client_counter + 1) == MAX_CLIENTS)
        {
            write(cliSd, apologise, sizeof(apologise));
            close(cliSd);
            //exit(EXIT_FAILURE);
        }

        client_t *client = (client_t *) calloc(1, sizeof(client_t));
        client->connect_fd = cliSd;

        // create thread for new client
        pthread_t cli_pid;
        if (pthread_create(&cli_pid, NULL, &handle_client, (void *) client) != 0)
        {
            perror("pthread_create() fail");
            exit(EXIT_FAILURE);
        }
        pthread_detach(cli_pid);
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


void *handle_client(void *arg)
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

    // some kludges for decoration:)
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