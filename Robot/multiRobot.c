#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "consoleUtils.h"

#define total_num_steps 1000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int _cur_leg = 0;
int _cur_step = 0;
int _total_legs_num = 0;


void *take_step(void *args)
{
    assert(args);

    int this_leg_num = *(int *) args;
    int total_leg_num = _total_legs_num;

    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (_cur_step == total_num_steps)
        {
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        else if (_cur_leg == this_leg_num)
        {
            _cur_leg++;
            _cur_step++;

            printf("Step %d, leg %d\n", _cur_step, this_leg_num + 1);
            _cur_leg = _cur_leg % total_leg_num;
        }

        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char *argv[])
{
    int legs_num = readNumber(argc, argv);
    if (legs_num <= 0)
    {
        printf("Expected non-negative value, got %d", legs_num);
        exit(EXIT_FAILURE);
    }

    // Uses global variable to do not send same information to each thread
    _total_legs_num = legs_num;

    pthread_t *legs = (pthread_t *) calloc(legs_num, sizeof(pthread_t));
    if (!legs)
    {
        perror("calloc() fail");
        exit(EXIT_FAILURE);
    }

    int *legs_id = (int *) calloc(legs_num, sizeof(size_t));
    if (!legs_id)
    {
        perror("calloc() fail");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < legs_num; i++)
    {
        legs_id[i] = i;
        if (pthread_create(&legs[i], NULL, take_step, &legs_id[i]) != 0)
        {
            fprintf(stderr, "pthread_create() fail\n");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < legs_num; i++)
    {
        if (pthread_join(legs[i], (void **) NULL) != 0)
        {
            fprintf(stderr, "pthread_join() fail\n");
            exit(EXIT_FAILURE);
        }
    }

    free(legs);
    free(legs_id);
    return 0;
}
