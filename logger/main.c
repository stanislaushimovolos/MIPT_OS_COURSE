#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const char firstUrlPrefix[] = "http://";
const char secondUrlPrefix[] = "https://";
const char errorFlag[] = "ERROR";


char **findUrl(char *data)
{
    assert(data);

    size_t errorCounter = 0;
    char *curPtr = data;

    // Count number of errors
    while ((curPtr = strstr(curPtr, errorFlag)) != NULL)
    {
        curPtr++;
        errorCounter++;
    }

    // Allocate memory for urls
    char **urls = (char **) calloc(errorCounter + 2, sizeof(char *));
    assert(urls);

    errorCounter = 0;
    curPtr = data;
    while ((curPtr = strstr(curPtr, errorFlag)) != NULL)
    {
        // Skip first argument
        urls[errorCounter + 1] = curPtr;
        curPtr++;
        errorCounter++;
    }

    for (int i = 0; i < errorCounter; i++)
    {
        // Skip first argument
        curPtr = strstr(urls[i + 1], firstUrlPrefix);
        urls[i + 1] = curPtr;

        if (!curPtr)
        {
            curPtr = strstr(curPtr, secondUrlPrefix);
            curPtr += sizeof(secondUrlPrefix);
            urls[i + 1] = curPtr;
        }
        else
            curPtr += sizeof(firstUrlPrefix);

        curPtr = strchr(curPtr, '/');
        *(curPtr + 1) = 0;
    }
    return urls;
}


char *getBuf(FILE *file, size_t *buf_sz)
{
    assert(file);
    fseek(file, 0, SEEK_END);
    size_t l_buf_sz = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *) calloc(l_buf_sz + 1, sizeof(char));
    fread(data, 1, l_buf_sz, file);

    if (buf_sz)
        *buf_sz = l_buf_sz;

    return data;
}


int main(int argc, char **argv, char **env)
{
    if (argc != 2)
    {
        printf("error, expected 1 argument, got %d\n", argc - 1);
        exit(EXIT_FAILURE);
    }

    errno = 0;
    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        printf("couldn't open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    char *data = getBuf(file, NULL);
    char **res = findUrl(data);

    // Assign first arg to name of this process
    res[0] = argv[0];

    // Call wget
    execvp("wget", res);

    return EXIT_FAILURE;
}