#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

static const char grepInstruction[] = " | grep -a ERROR | grep  -Eao \"https?://\\S+?\\/|http?://\\S+?\\/\"  > ";
static const char getInstruction[] = " && wget -i ";
static const char catInstruction[] = "cat ";
static const char defaultOutput[] = "error_urls.log";

int main(int argc, char **argv, char **env)
{
    if (argc != 2)
    {
        printf("error, expected 1 argument, got %d\n", argc - 1);
        exit(EXIT_FAILURE);
    }

    char *inputFile = argv[1];

    // Check if file exists
    FILE *file = fopen(inputFile, "rt");
    if (!file)
    {
        printf("can't open %s\n", inputFile);
        exit(EXIT_FAILURE);
    }

    // Compute size if buffer
    size_t instructionSize =
            strlen(argv[1]) + sizeof(catInstruction) +
            sizeof(grepInstruction) + sizeof(getInstruction) + 2 * sizeof(defaultOutput);

    char *buffer = (char *) calloc(instructionSize + 1, sizeof(char));

    /* ............................
     * ............................
     * ............................
     * */

    strcat(buffer, catInstruction);
    strcat(buffer, inputFile);
    strcat(buffer, grepInstruction);
    strcat(buffer, defaultOutput);
    strcat(buffer, getInstruction);
    strcat(buffer, defaultOutput);

    int result = 0;
    result = system(buffer);

    free(buffer);
    return result;
}