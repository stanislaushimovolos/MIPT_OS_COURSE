#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#define size_of_dish 16
#define max_thought 3
#define max_eating 4
#define phil_n 5

void take_forks(int phil_pos, int forks_id, int waiter_id);

void put_forks(int phil_pos, int forks_id, int waiter_id);

void think(int id);

void eat(int id);


int main()
{
    int waiter_id = 0;
    int forks_id = 0;

    struct sembuf waiter = {};
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

    // Number(name) of current process(philosopher)
    int phil_id = 0;
    for (phil_id = 0; phil_id < phil_n; phil_id++)
    {
        int pid = fork();
        if (pid == 0)
            break;

        else if (phil_id == phil_n - 1)
        {
            int status = 0;
            for (int i = 0; i < phil_n; i++)
                if (wait(&status) < -1)
                {
                    perror("wait() fail");
                    exit(EXIT_FAILURE);
                }

            return 0;
        }
    }

    // life cycle of usual philosopher
    for (int i = 0; i < size_of_dish; i++)
    {
        think(phil_id);
        take_forks(phil_id, forks_id, waiter_id);

        eat(phil_id);
        put_forks(phil_id, forks_id, waiter_id);
    }

    return 0;
}


void take_forks(int phil_pos, int forks_id, int waiter_id)
{
    struct sembuf sem_inc_dec = {};

    // Call waiter
    sem_inc_dec.sem_op = -1;
    sem_inc_dec.sem_flg = 0;
    sem_inc_dec.sem_num = 0;

    if (semop(waiter_id, &sem_inc_dec, 1) < 0)
    {
        printf("Can\'t wait for condition \n");
        exit(EXIT_FAILURE);
    }

    // Take left fork
    sem_inc_dec.sem_op = -1;
    sem_inc_dec.sem_num = phil_pos;

    if (semop(forks_id, &sem_inc_dec, 1) < 0)
    {
        printf("Can\'t wait for condition take\n");
        exit(EXIT_FAILURE);
    }

    // Take right fork
    sem_inc_dec.sem_op = -1;
    sem_inc_dec.sem_num = (phil_pos + 1) % phil_n;

    if (semop(forks_id, &sem_inc_dec, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }
}


void put_forks(int phil_pos, int forks_id, int waiter_id)
{
    struct sembuf sem_inc_dec = {};

    // Call waiter
    sem_inc_dec.sem_flg = 0;
    sem_inc_dec.sem_op = 1;
    sem_inc_dec.sem_num = (phil_pos + 1) % phil_n;

    if (semop(forks_id, &sem_inc_dec, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }

    // Put left fork
    sem_inc_dec.sem_op = 1;
    sem_inc_dec.sem_num = phil_pos;

    if (semop(forks_id, &sem_inc_dec, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }

    // Put right fork
    sem_inc_dec.sem_op = 1;
    sem_inc_dec.sem_num = 0;

    if (semop(waiter_id, &sem_inc_dec, 1) < 0)
    {
        printf("Can\'t wait for condition\n");
        exit(EXIT_FAILURE);
    }
}


void think(int id)
{
    printf("Philosopher № %d is thinking\n", id);
    sleep(rand() % (max_thought + 1));
}


void eat(int id)
{
    printf("Philosopher № %d is eating\n", id);
    sleep(rand() % (max_eating + 1));
}