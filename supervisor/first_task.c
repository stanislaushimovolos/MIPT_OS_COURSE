#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

#include "consoleUtils.h"


int main(int argc, char *argv[])
{
    int max_process_num = readNumber(argc, argv);
    int cur_process_num = 0;
    pid_t status = 0;

    while (1)
    {
        if (cur_process_num < max_process_num)
        {
            status = fork();
            if (status == -1)
                return EXIT_FAILURE;
            if (status == 0)
                break;

            cur_process_num++;
        }

        status = waitpid(-1, NULL, WNOHANG);
        if (status != 0)
            cur_process_num--;
    }

    int sec = rand() % 6;
    sleep(sec);

    pid_t pid = getpid();
    printf("PID = %d\n", pid);

    return rand() % 2;
}