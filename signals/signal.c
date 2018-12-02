#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define message 1
#define base 10
#define n_bits 8

unsigned char **create_storage(int num, int *size_of_num);

int destruct_storage(unsigned char **storage, int storage_sz);

void fill_byte(unsigned char *storage, unsigned char value);

void receiver_handler(int);

void sender_handler(int);


int indicator = 0;

int got = 0;

int ppid = 0;

int current_val = 0;


int main()
{
    signal(SIGUSR1, sender_handler);
    ppid = getpid();

    int old_ind = indicator;

    int pid = fork();
    if (pid == 0)
    {
        signal(SIGUSR1, receiver_handler);
        signal(SIGUSR2, receiver_handler);


        int n_digits = 0;
        int n_got = 0;

        while (1)
        {
            //Means ready
            kill(ppid, SIGUSR1);
            while (got == 0);
            got = 0;

            n_digits += pow(2, n_got) * current_val;
            n_got++;

            if (n_got == n_bits)
            {
                printf("number of digits = %d\n", n_digits);
                break;
            }
        }

        int global_digit = 0;
        for (int i = 0; i < n_digits; i++)
        {
            int cur_digit = 0;
            n_got = 0;
            got = 0;

            while (1)
            {
                kill(ppid, SIGUSR1);
                while (got == 0);
                got = 0;

                cur_digit += pow(2, n_got) * current_val;
                n_got++;

                if (n_got == n_bits)
                {
                    global_digit += cur_digit * pow(10, n_digits - i - 1);
                    break;
                }
            }
        }
        printf("got digit = %d\n", global_digit);
    }

    else
    {
        int digits_num = 0;
        unsigned char **storage = create_storage(message, &digits_num);

        while (indicator != 1);

        for (int i = 0; i < n_bits; i++)
        {
            while (old_ind == indicator);
            old_ind = indicator;
            if ((digits_num & (1u << i)) == 0)
                kill(pid, SIGUSR1);
            else
                kill(pid, SIGUSR2);
        }


        while (old_ind == indicator);
        for (int i = 0; i < digits_num; i++)
            for (int j = 0; j < n_bits; j++)
            {
                while (old_ind == indicator);
                old_ind = indicator;
                if (storage[i][j] == 0)
                    kill(pid, SIGUSR1);
                else
                    kill(pid, SIGUSR2);
            }

        destruct_storage(storage, digits_num);
    }


    return 0;
}


void sender_handler(int sign)
{
    //printf("send\n");
    switch (sign)
    {
        case SIGUSR1:
        {
            indicator++;
            break;
        }
        default:
            break;
    }
}


void receiver_handler(int sign)
{
    //printf("received\n");
    got = 1;
    switch (sign)
    {
        case SIGUSR1:
        {
            current_val = 0;
            break;
        }
        case SIGUSR2:
        {
            current_val = 1;
            break;
        }
        default:
            break;
    }
}


int destruct_storage(unsigned char **storage, int storage_sz)
{
    assert(storage);

    for (int i = 0; i < storage_sz; i++)
    {
        free(storage[i]);
    }
    free(storage);
}


void fill_byte(unsigned char *storage, unsigned char val)
{
    unsigned char tmp = 0;
    for (int i = 0; i < n_bits; i++)
    {
        tmp = ((val & (1u << i)) != 0);
        storage[i] = tmp;
    }
}


unsigned char **create_storage(int num, int *size_of_num)
{
    int tmp_num = num;
    int digit_counter = 0;

    while (tmp_num != 0)
    {
        tmp_num = tmp_num / base;
        digit_counter++;
    }

    if (size_of_num != NULL)
        *size_of_num = digit_counter;

    unsigned char **number_in_bits = (unsigned char **) calloc(digit_counter + 1, sizeof(char *));
    for (int i = 0; i < digit_counter; i++)
        number_in_bits[i] = (unsigned char *) calloc(n_bits, sizeof(char));

    tmp_num = num;
    for (int i = 0; i < digit_counter; i++)
    {
        fill_byte(number_in_bits[digit_counter - i - 1], tmp_num % 10);
        tmp_num = tmp_num / 10;

    }
    return number_in_bits;
}


