#include <iostream>
#include <cerrno>
#include <unistd.h>
#include <sys/wait.h>
#include <ctime>
#include <map>

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

    std::map<int, int> dict;

    while (true)
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
            if (pid == -1)
            {
                printf("Can't create new process\n");
                exit(EXIT_FAILURE);
            }

            close(fd[0]);
            write(fd[1], &message, sizeof(int));

            dict[pid] = message;

            //Increment current number of active processes
            cur_process_num++;

            //Increment message
            message++;
        }

        // Track errors
        pid = waitpid(-1, &status, WNOHANG);
        if (pid != 0)
        {
            if (WEXITSTATUS(status) == 1)
            {
                int error_msg = dict[pid];
                dict.erase(pid);

                if (pipe(fd) < 0)
                {
                    printf("Can\'t create pipe\n");
                    exit(EXIT_FAILURE);
                }

                pid = fork();
                if (pid == 0)
                    break;
                if (pid == -1)
                {
                    printf("Can't create new process\n");
                    exit(EXIT_FAILURE);
                }

                dict[pid] = error_msg;

                close(fd[0]);
                write(fd[1], &error_msg, sizeof(int));
            }
            else
                cur_process_num--;
        }
    }

    close(fd[1]);

    unsigned int msg = 0;
    read(fd[0], &msg, sizeof(int));

    // Msg as seed
    srand((unsigned) time(nullptr) + msg);

    // Sleep ..zzzz..
    auto sec = random() % 6u + 2u;
    sleep(sec);

    status = random() % 2;

    pid_t child_pid = getpid();
    printf("PID = %d msg = %d status = %d\n", child_pid, msg, status);

    return status;
}
