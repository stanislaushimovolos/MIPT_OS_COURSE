#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const char urlPrefix1[] = "http://";
const char urlPrefix2[] = "https://";
const char errorFlag[] = "ERROR";


char **findUrl(char *data)
{
    size_t errCounter = 0;

    char *ptr = data;
    while ((ptr = strstr(ptr, errorFlag)) != NULL)
    {
        ptr++;
        errCounter++;
    }

    char **urls = (char **) calloc(errCounter + 2, sizeof(char *));

    int i = 0;
    ptr = data;
    while ((ptr = strstr(ptr, errorFlag)) != NULL)
    {
        urls[i + 1] = ptr;
        ptr++;
        i++;
    }


    for (int i = 0; i < errCounter; i++)
    {
        ptr = strstr(urls[i + 1], urlPrefix1);
        urls[i + 1] = ptr;

        if (!ptr)
        {
            ptr = strstr(ptr, urlPrefix2);
            ptr += sizeof(urlPrefix2);
            urls[i + 1] = ptr;
        }
        else
            ptr += sizeof(urlPrefix1);

        ptr = strchr(ptr, '/');
        *(ptr + 1) = 0;
        ptr += 2;
    }

    urls[errCounter] = NULL;

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
    res[0] = argv[0];

    int i = 0;
    execvp("wget",  res);
    return -1;
}