#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdlib.h>
#define SHMKEY 75
#define K 1024
#define SEM_ID 225
#define SEM_FULL 128
#define SEM_EMPTY 256
int shmid;
int sem_set_id;
int sem_full;
int sem_empty;
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};
struct buf
{
    int num;
    int read;
    int write;
    int buffer[5];
};
void Producer()
{
    char argv[2];
    for (int i = 0; i < 2; i++)
    {
        if (fork() == 0)
        {
            argv[0] = i + 48;
            argv[1] = 0;
            int ret = execl("producer", "producer", argv, 0);
            if (ret == -1)
            {
                printf("Producer start fail\n");
                exit(0);
            }
        }
    }
}
void Consumer()
{
    char argv[2];
    for (int i = 0; i < 3; i++)
    {
        if (fork() == 0)
        {
            argv[0] = i + 48;
            argv[1] = 0;
            int ret = execl("consumer", "consumer", argv, 0);
            if (ret == -1)
            {
                printf("Consumer start fail\n");
                exit(0);
            }
        }
    }
}
int main(int argc, char *argv[])
{
    int rc;
    struct buf *pint;
    char *addr;
    union semun sem_val;
    sem_set_id = semget(SEM_ID, 1, IPC_CREAT | 0600);
    sem_full = semget(SEM_FULL, 3, IPC_CREAT | 0600);
    sem_empty = semget(SEM_EMPTY, 3, IPC_CREAT | 0600);
    sem_val.val = 1;
    rc = semctl(sem_set_id, 0, SETVAL, sem_val);
    sem_val.val = 3;
    rc = semctl(sem_empty, 0, SETVAL, sem_val);
    sem_val.val = 0;
    rc = semctl(sem_full, 0, SETVAL, sem_val);
    shmid = shmget(SHMKEY, K, 0777 | IPC_CREAT);
    addr = shmat(shmid, 0, 0);
    pint = (struct buf *)addr;
    memset(addr, 0, 512);
    shmdt(addr);
    Producer();
    Consumer();
    printf("Create success!\n");
    while (wait(NULL) != -1);
    printf("-----all end-----");
    shmctl(shmid, IPC_RMID, NULL);
    semctl(sem_set_id, IPC_RMID, NULL);
    semctl(sem_full, IPC_RMID, NULL);
    semctl(sem_empty, IPC_RMID, NULL);
    return 0;
}