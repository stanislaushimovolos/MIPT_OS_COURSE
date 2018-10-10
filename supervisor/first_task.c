#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

int readNumber(int argc, char *argv[])
{
    assert(argv);
    if (argc < 2)
    {
        printf("error, expected 1 number\n");
        exit(EXIT_FAILURE);
    }

    char *str = argv[1];
    char *endPtr;

    errno = 0;
    int val = (int) strtol(str, &endPtr, 10);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
        || (errno != 0 && val == 0))
    {
        printf("error, can't read number\n");
        exit(EXIT_FAILURE);
    }

    if (endPtr == str)
    {
        printf("No digits were found\n");
        return -1;
    }

    if (*endPtr != '\0')
        printf("Further characters after number: %s\n", endPtr);

    return val;

}

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