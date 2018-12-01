#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <semaphore.h>

#define phil_num 5
#define max_dur_of_thought 3
#define max_dur_of_eating 4

void *philosopher(void *args);

void take_forks(int id);

void put_forks(int id);

void think(int id);

void eat(int id);

pthread_mutex_t forks[phil_num];

sem_t waiter;


int main()
{
    pthread_t phil[phil_num];
    int args[phil_num];

    sem_init(&waiter, 0, phil_num - 1);

    for (int i = 0; i < phil_num; i++)
    {
        args[i] = i;
        if (pthread_create(&phil[i], NULL, philosopher, &args[i]) != 0)
        {
            fprintf(stderr, "pthread_create() fail\n");
            exit(EXIT_FAILURE);
        }
    }

    // Never happens
    for (int i = 0; i < phil_num; i++)
        pthread_join(phil[i], (void **) NULL);
    return 0;
}


void *philosopher(void *args)
{
    assert(args);
    int id = *(int *) args;

    while (1)
    {
        think(id);
        take_forks(id);

        eat(id);
        put_forks(id);
    }
}


void take_forks(int id)
{
    sem_wait(&waiter);
    pthread_mutex_lock(&forks[id]);
    pthread_mutex_lock(&forks[(id + 1) % phil_num]);
}


void put_forks(int id)
{
    pthread_mutex_unlock(&forks[id]);
    pthread_mutex_unlock(&forks[(id + 1) % phil_num]);
    sem_post(&waiter);
}


void think(int id)
{
    printf("Philosopher № %d is thinking\n", id);
    sleep(rand() % (max_dur_of_thought) + 1);
}


void eat(int id)
{
    printf("Philosopher № %d is eating\n", id);
    sleep(rand() % (max_dur_of_eating) + 1);
}