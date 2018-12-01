#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <semaphore.h>

#define phil_n 5
#define max_dur_eating 4
#define max_dur_thought 3

void *philosopher(void *args);

void take_forks(int id);

void put_forks(int id);

void think(int id);

void eat(int id);

pthread_mutex_t forks[phil_n];
sem_t waiter;


int main()
{
    int args[phil_n] = {};
    pthread_t phil[phil_n] = {};

    if (sem_init(&waiter, 0, phil_n - 1) < 0)
    {
        perror("sem_init() fail");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < phil_n; i++)
    {
        args[i] = i;
        if (pthread_create(&phil[i], NULL, philosopher, &args[i]) != 0)
        {
            fprintf(stderr, "pthread_create() fail\n");
            exit(EXIT_FAILURE);
        }
    }

    // Never happens
    for (int i = 0; i < phil_n; i++)
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
    if (sem_wait(&waiter) < 0)
    {
        perror("sem_wait() fail");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&forks[id]);
    pthread_mutex_lock(&forks[(id + 1) % phil_n]);
}


void put_forks(int id)
{
    pthread_mutex_unlock(&forks[(id + 1) % phil_n]);
    pthread_mutex_unlock(&forks[id]);

    if (sem_post(&waiter) < 0)
    {
        perror("sem_post() fail");
        exit(EXIT_FAILURE);
    }
}


void think(int id)
{
    printf("Philosopher № %d is thinking\n", id);
    sleep(rand() % (max_dur_thought + 1));
}


void eat(int id)
{
    printf("Philosopher № %d is eating\n", id);
    sleep(rand() % (max_dur_eating + 1));
}