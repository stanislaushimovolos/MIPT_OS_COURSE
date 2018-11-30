#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define phil_num 5
#define max_dur_of_thought 3
#define max_dur_of_eating 4

void *philosopher(void *args);

void take_forks(int id);

void put_forks(int id);

void think(int id);

void eat(int id);


pthread_mutex_t forks[phil_num];
struct sembuf waiter_sem_buf;
int waiter_id;


int main()
{
    pthread_t phil[phil_num];
    int args[phil_num] = {0};

    // Initialize waiter
    waiter_id = semget(IPC_PRIVATE, phil_num - 1, IPC_CREAT);
    waiter_sem_buf.sem_num = 0;
    waiter_sem_buf.sem_flg = 0;

    for (int i = 0; i < phil_num; i++)
    {
        args[i] = i;
        pthread_create(&phil[i], NULL, philosopher, &args[i]);
    }
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
    waiter_sem_buf.sem_num = 1;
    semop(waiter_id, &waiter_sem_buf, 1);

    pthread_mutex_lock(&forks[id]);
    pthread_mutex_lock(&forks[(id + 1) % phil_num]);
}


void put_forks(int id)
{
    pthread_mutex_unlock(&forks[id]);
    pthread_mutex_unlock(&forks[(id + 1) % phil_num]);

    waiter_sem_buf.sem_num = -1;
    semop(waiter_id, &waiter_sem_buf, 1);

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