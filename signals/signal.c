#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define message 10050069
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

// last received bit
int cur_bit = 0;

// means that receiver got a bit
int received_flag = 0;


int main()
{
    p_pid = getpid();

    // uses to check the incoming signal
    char send_flag_old = send_flag;

    //define SIGUSR1 to confirm the receiver readiness
    signal(SIGUSR1, sender_handler);

    int pid = fork();
    if (pid == 0)
    {
        // SIGUSR1 => 0, SIGUSR1 => 1
        signal(SIGUSR1, receiver_handler);
        signal(SIGUSR2, receiver_handler);

#define receive_byte(var)                       \
    for (int k = 0; k < n_bits; k++)            \
        {                                       \
            kill(p_pid, SIGUSR1);               \
            while (received_flag == 0);         \
            received_flag = 0;                  \
                                                \
            (var) += pow(2, k) * cur_bit;       \
        }

        int n_bytes = 0;
        receive_byte(n_bytes);

        printf("number of digits = %d\n", n_bytes);

        int num = 0;
        for (int i = 0; i < n_bytes; i++)
        {
            int cur_byte = 0;

            received_flag = 0;
            receive_byte(cur_byte);

            num += cur_byte * pow(10, n_bytes - i - 1);
        }

        printf("msg = %d\n", num);
    }

#undef receive_byte

        // receiver part
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
            perror("kill() fail");              \
    }                                           \
    else                                        \
    {                                           \
        if (kill(pid, SIGUSR1) < 0)             \
            perror("kill() fail");              \
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
#undef get_bit


void receiver_handler(int sign)
{
    received_flag = 1;
    switch (sign)
    {
        case SIGUSR1:
        {
            cur_bit = 0;
            break;
        }
        case SIGUSR2:
        {
            cur_bit = 1;
            break;
        }
        default:
            break;
    }
}


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


