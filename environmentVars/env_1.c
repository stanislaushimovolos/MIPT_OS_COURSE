#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[], char *env[])
{
    char *user_name = getlogin();
    if (!user_name)
    {
        printf("Can't determine user's name\n");
        exit(EXIT_FAILURE);
    }

    while (*env)
    {
        if (strstr(*env, user_name))
            printf("%s\n", *env);
        env++;
    }
    return EXIT_SUCCESS;
}