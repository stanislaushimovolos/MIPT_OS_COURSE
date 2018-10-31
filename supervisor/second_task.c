#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#include "consoleUtils.h"


int main(int argc, char *argv[])
{
    //int max_process_num = readNumber(argc, argv);

    int max_process_num = 5;
    int cur_process_num = 0;
    pid_t pid = 0;

    int fd[2] = {0};

    int message = 0;
    int status = 0;

    while (1)
    {
        if (cur_process_num < max_process_num)
        {
            if (pipe(fd) < 0)
            {
                printf("Can\'t create pipe\n");
                exit(EXIT_FAILURE);
            }

            pid = fork();
            if (pid == 0)
                break;

            close(fd[0]);
            write(fd[1], &message, sizeof(int));

            //Increment current number of active processes
            cur_process_num++;

            //Increment message
            message++;
        }

        // Track errors
        pid = waitpid(-1, &status, WNOHANG);
        if (pid != 0)
        {
            while (WEXITSTATUS(status))
            {
                pipe(fd);
                pid = fork();

                if (pid == 0)
                {
                    close(fd[1]);

                    unsigned int msg = 0;
                    read(fd[0], &msg, sizeof(int));

                    // Msg as seed
                    srand(msg);

                    // Sleep ..zzzz..
                    unsigned int sec = rand() % 6u + 7u;
                    sleep(sec);

                    status = rand() % 2;

                    pid_t child_pid = getpid();
                    printf("PID = %d msg = %d status = %d\n", child_pid, msg, status);

                    return status;

                }

                close(fd[0]);
                write(fd[1], &message, sizeof(int));

            }
            cur_process_num--;
        }

    }

    close(fd[1]);

    unsigned int msg = 0;
    read(fd[0], &msg, sizeof(int));

    // Msg as seed
    srand(msg);

    // Sleep ..zzzz..
    unsigned int sec = rand() % 6u + 7u;
    sleep(sec);

    status = rand() % 2;

    pid_t child_pid = getpid();
    printf("PID = %d msg = %d status = %d\n", child_pid, msg, status);

    return status;
}
