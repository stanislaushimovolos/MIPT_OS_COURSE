#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "consoleUtils.h"

#define RIGHT 1
#define LEFT 0

int ready[2] = {0, 0};
int turn = -1;

void *rightLeg(void *args)
{
    int total_steps = *(int *) args;
    int step_counter = 0;

    while (step_counter < total_steps)
    {
        ready[RIGHT] = 1;
        turn = LEFT;
        while (ready[LEFT] && turn == LEFT);

        printf("right\n");
        ready[RIGHT] = 0;
        step_counter++;
    }
}


void *leftLeg(void *args)
{
    int total_steps = *(int *) args;
    int step_counter = 0;

    while (step_counter < total_steps)
    {
        ready[LEFT] = 1;
        turn = RIGHT;
        while (ready[RIGHT] && turn == RIGHT);

        printf("left\n");
        ready[LEFT] = 0;
        step_counter++;
    }
}


int main(int argc, char *argv[])
{
    pthread_t right_leg, left_leg;

    int steps = readNumber(argc, argv);
    if (steps == -1)
        exit(EXIT_FAILURE);

    int status = pthread_create(&right_leg, NULL, rightLeg, &steps);
    if (status != 0)
        exit(EXIT_FAILURE);

    status = pthread_create(&left_leg, NULL, leftLeg, &steps);
    if (status != 0)
        exit(EXIT_FAILURE);


    pthread_join(right_leg, (void **) NULL);
    pthread_join(left_leg, (void **) NULL);
    return 0;
}