#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define thread_num 100

void critical_section(int thread_id);

void unlock(int thread_id);

void lock(int thread_id);

void *thread(void *arg);

int number[thread_num] = {0};
int choosing[thread_num] = {0};
int resource = 0;


int main(int argc, char **argv)
{
    pthread_t threads[thread_num];
    for (int i = 0; i < thread_num; ++i)
        if (pthread_create(&threads[i], NULL, &thread, (void *) (size_t)i) < 0)
        {
            fprintf(stderr, "pthread_create() fail\n");
            exit(EXIT_FAILURE);
        }

    // never happens
    for (int i = 0; i < thread_num; ++i)
        pthread_join(threads[i], NULL);
}


void lock(int thread_id)
{
    // to make the next operation(choose ticket) atomic
    choosing[thread_id] = 1;

    int max_number = 0;
    for (int i = 0; i < thread_num; ++i)
        max_number = number[i] > max_number ? number[i] : max_number;

    // set maximum value to the ticket
    number[thread_id] = max_number + 1;
    choosing[thread_id] = 0;

    for (int other = 0; other < thread_num; ++other)
    {
        // wait until thread[other] receives its number
        while (choosing[other]);

        /* (number[other] == 0) means that thread[other] left critical section
         * second condition is used to wait for until all threads with a smaller number or with the same number,
           but with a higher priority, finish their activity
         * */
        while (number[other] != 0 &&
               (number[other] < number[thread_id] || (number[other] == number[thread_id] && other < thread_id)));
    }
}


void unlock(int thread_id)
{
    number[thread_id] = 0;
}


void critical_section(int thread_id)
{
    if (resource != 0)
    {
        printf("Something is wrong with the algorithm, thread № %d, resource = %d",
               thread_id, resource);
        exit(EXIT_FAILURE);
    }

    resource = thread_id;
    printf("%d is in the critical section\n", resource);
    resource = 0;
}


void *thread(void *arg)
{
    size_t thread = (size_t) arg;
    while (1)
    {
        lock(thread);
        critical_section(thread);
        unlock(thread);
    }
}

