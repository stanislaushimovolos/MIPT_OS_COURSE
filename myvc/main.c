#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

//  -w print words
//  -l print lines
static const char const commandLineParams[] = "lw";

int printWordsFlag = 0;
int printLinesFlag = 0;

char *getBuf(FILE *file, size_t *buf_sz);

int processFile(char *fileName);


int main(int argc, char *argv[])
{

    // Parse command line
    int opt = 0;
    while ((opt = getopt(argc, argv, commandLineParams)) != -1)
    {
        switch (opt)
        {
            case 'l':
            {
                printLinesFlag = 1;
                break;
            }
            case 'w':
            {
                printWordsFlag = 1;
                break;
            }
            case '?':
            case ':':
            default:
            {
                printf("Unknown flag %c", opt);
                exit(EXIT_FAILURE);
            }
        }
    }

    while (argv[optind])
    {
        // Analyze files
        processFile(argv[optind]);
        optind++;
    }
    return EXIT_SUCCESS;
}


int processFile(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (!file)
    {
        printf("mywc: %s: No such file or directory \n", fileName);
        return -1;
    }

    size_t sizeofFile = 0;
    char *data = getBuf(file, &sizeofFile);
    fclose(file);

    if (!data)
    {
        printf("Couldn't allocate memory for file's content: %s\n", fileName);
        return -1;
    }

    char *data_ptr = data;
    while (*data_ptr && (*data_ptr == '\n' || *data_ptr == ' '))
        data_ptr++;

    int wordCounter = 0;
    int lineCounter = 0;

    int wordFlag = 0;
    while (*data_ptr)
    {
        if ((*data_ptr != ' ' && *data_ptr != '\n' && *data_ptr != '\t') && wordFlag == 0)
        {
            wordFlag = 1;
            wordCounter++;
        }
        else if (*data_ptr == ' ' || *data_ptr == '\n' || *data_ptr == '\t')
        {
            if (*data_ptr == '\n')
                lineCounter++;
            wordFlag = 0;
        }
        data_ptr++;
    }

    if (printLinesFlag)
        printf("%d ", lineCounter);
    if (printWordsFlag)
        printf("%d ", wordCounter);
    if (!printLinesFlag && !printWordsFlag)
        printf("%d %d %lu ", lineCounter, wordCounter, sizeofFile);

    printf("%s\n", fileName);
    return 0;
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