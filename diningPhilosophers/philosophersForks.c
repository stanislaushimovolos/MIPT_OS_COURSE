#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#define phil_n 5
#define max_dur_eating 4
#define max_dur_thought 3

void *philosopher(void *args);

void take_forks(int phil_pos, int forks_id, int waiter_id);

void put_forks(int phil_name, int forks_id, int waiter_id);

void think(int id);

void eat(int id);


int main()
{
    int waiter_id, forks_id;

    struct sembuf waiter;
    if ((waiter_id = semget(IPC_PRIVATE, 1, 666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get semid\n");
        exit(EXIT_FAILURE);
    }


    waiter.sem_op = phil_n - 1;
    waiter.sem_flg = 0;
    waiter.sem_num = 0;

    if (semop(waiter_id, &waiter, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }

    struct sembuf forks[phil_n] = {};
    for (int i = 0; i < phil_n; i++)
    {
        forks[i].sem_op = 1;
        forks[i].sem_num = i;
        forks[i].sem_flg = 0;
    }

    if ((forks_id = semget(IPC_PRIVATE, phil_n, 666 | IPC_CREAT)) < 0)
    {
        printf("Can\'t get semid\n");
        exit(EXIT_FAILURE);
    }

    if (semop(forks_id, forks, phil_n) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }

    int id = 0;
    for (id = 0; id < phil_n; id++)
    {
        int pid = fork();
        if (pid == 0)
            break;
        else if (id == phil_n - 1)
            wait_pid
    }

    while (1)
    {
        think(id);
        take_forks(id, forks_id, waiter_id);

        eat(id);
        put_forks(id, forks_id, waiter_id);
    }
}


void take_forks(int phil_pos, int forks_id, int waiter_id)
{
    struct sembuf waiter;
    waiter.sem_op = -1;
    waiter.sem_flg = 0;
    waiter.sem_num = 0;

    if (semop(waiter_id, &waiter, 1) < 0)
    {
        printf("Can\'t wait for condition \n");
        exit(EXIT_FAILURE);
    }


    struct sembuf left_fork;
    left_fork.sem_flg = 0;
    left_fork.sem_op = -1;
    left_fork.sem_num = phil_pos;

    if (semop(forks_id, &left_fork, 1) < 0)
    {
        printf("Can\'t wait for condition take\n");
        exit(EXIT_FAILURE);
    }

    struct sembuf right_fork;
    right_fork.sem_flg = 0;
    right_fork.sem_op = -1;
    right_fork.sem_num = (phil_pos + 1) % phil_n;

    if (semop(forks_id, &right_fork, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }
}


void put_forks(int phil_pos, int forks_id, int waiter_id)
{
    struct sembuf right_fork;
    right_fork.sem_flg = 0;
    right_fork.sem_op = 1;
    right_fork.sem_num = (phil_pos + 1) % phil_n;

    if (semop(forks_id, &right_fork, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }

    struct sembuf left_fork;
    left_fork.sem_flg = 0;
    left_fork.sem_op = 1;
    left_fork.sem_num = phil_pos;

    if (semop(forks_id, &left_fork, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }

    struct sembuf waiter;
    waiter.sem_op = 1;
    waiter.sem_flg = 0;
    waiter.sem_num = 0;

    if (semop(waiter_id, &waiter, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
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