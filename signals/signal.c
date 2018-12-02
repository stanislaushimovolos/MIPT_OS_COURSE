#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define message 100500228
#define n_bits 8
#define base 10

#define get_bit(val, pos) ((val) & (1u << (pos)))

unsigned char *create_storage(int num, int *size_of_num);

void receiver_handler(int);

void sender_handler(int);

// pid of parent
int p_pid = 0;

// uses to check the incoming signal
char send_flag = 0;


int got = 0;

int current_val = 0;


int main()
{
    p_pid = getpid();

    // uses to check the incoming signal
    char send_flag_old = send_flag;

    // to confirm the receiver readiness
    signal(SIGUSR1, sender_handler);

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
            kill(p_pid, SIGUSR1);
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
                kill(p_pid, SIGUSR1);
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
        printf("got number = %d\n", global_digit);
    }


    else
    {

#define send_msg(byte)                          \
for (int k = 0; k < n_bits; k++)                \
{                                               \
    /* to confirm the receiver readiness */     \
    while (send_flag_old == send_flag);         \
    send_flag_old = send_flag;                  \
                                                \
    if (get_bit(byte, k))                       \
    {                                           \
        if (kill(pid, SIGUSR2) < 0)             \
        perror("kill() fail");                  \
    }                                           \
    else                                        \
    {                                           \
        if (kill(pid, SIGUSR1) < 0)             \
        perror("kill() fail");                  \
    }                                           \
}

        int msg_sz = 0;
        unsigned char *msg_byte = create_storage(message, &msg_sz);

        // send number of bytes in message
        send_msg(msg_sz);

        // send message
        for (int i = 0; i < msg_sz; i++)
            send_msg(msg_byte[i])

        free(msg_byte);
    }
    return 0;
}

#undef send_msg


void sender_handler(int sign)
{
    switch (sign)
    {
        case SIGUSR1:
        {
            send_flag = !send_flag;
            break;
        }
        default:
            break;
    }
}


void receiver_handler(int sign)
{
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


unsigned char *create_storage(int num, int *size_of_num)
{
    int tmp_num = num;
    int digit_counter = 0;

    while (tmp_num != 0)
    {
        tmp_num = tmp_num / base;
        digit_counter++;
    }

    if (size_of_num)
        *size_of_num = digit_counter;

    //12345 => [1, 2, 3, 4, 5]
    unsigned char *digit_repr = (unsigned char *) calloc((size_t) digit_counter + 1, sizeof(char *));
    if (digit_repr == 0)
    {
        fprintf(stderr, "calloc() fail");
        exit(EXIT_FAILURE);
    }

    tmp_num = num;
    for (int i = 0; i < digit_counter; i++)
    {
        digit_repr[digit_counter - i - 1] = tmp_num % base;
        tmp_num = tmp_num / base;
    }

    return digit_repr;
}


