#include "consoleUtils.h"


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
