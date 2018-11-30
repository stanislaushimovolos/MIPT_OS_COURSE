#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "consoleUtils.h"

#define total_num_steps 1000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

size_t _cur_leg = 0;
size_t _cur_step = 0;
size_t _total_legs_num = 0;


void *make_step(void *args)
{
    size_t this_leg_num = *(size_t *) args;
    size_t total_leg_num = _total_legs_num;

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

            printf("Step %lu, leg %lu\n", _cur_step, this_leg_num + 1);
            _cur_leg = _cur_leg % total_leg_num;
        }

        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char *argv[])
{

    size_t steps_num = (size_t) readNumber(argc, argv);

    // Uses global variable to do not send same information to each thread
    _total_legs_num = steps_num;

    pthread_t *legs = (pthread_t *) calloc(steps_num, sizeof(pthread_t));
    size_t *legs_id = (size_t *) calloc(steps_num, sizeof(size_t));

    for (int i = 0; i < steps_num; i++)
    {
        legs_id[i] = i;
        pthread_create(&legs[i], NULL, make_step, &legs_id[i]);
    }
    for (int i = 0; i < steps_num; i++)
        pthread_join(legs[i], (void **) NULL);

    free(legs);
    free(legs_id);
    return 0;
}
