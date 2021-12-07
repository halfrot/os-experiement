#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>
#define SHMKEY 75
#define K 1024
#define SEM_ID 225
#define SEM_FULL 128
#define SEM_EMPTY 256
int shmid;
int sem_set_id;
int sem_full;
int sem_empty;
struct buf
{
    int num;
    int read;
    int write;
    char buffer[5];
};
int main(int argc, char *argv[])
{
    struct buf *pint;
    char *addr;
    struct sembuf sem_op;
    int n;
    n = atoi(argv[1]);
    printf("success!\n");
    sem_set_id = semget(SEM_ID, 1, 0600);
    sem_full = semget(SEM_FULL, 1, 0600);
    sem_empty = semget(SEM_EMPTY, 1, 0600);
    for (int i = 0; i < 4; i++)
    {
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        semop(sem_full, &sem_op, 1);
        srand((unsigned)time(0));
        int r = 1 + rand() % 5;
        sleep(r);
        sem_op.sem_num = 0;
        sem_op.sem_op = -1;
        sem_op.sem_flg = 0;
        semop(sem_set_id, &sem_op, 1);
        time_t now;
        struct tm *timenow;
        time(&now);
        timenow = localtime(&now);
        shmid = shmget(SHMKEY, K, 0777);
        if (shmid < 0)
        {
            printf("shmget error\n");
            exit(-1);
        }
        addr = shmat(shmid, 0, 0);
        pint = (struct buf *)addr;
        pint->buffer[pint->read] = '#';
        pint->read = (pint->read + 1) % 3;
        pint->num--;
        printf("%02d:%02d:%02d   Consumer[%d] success  %d left ", timenow->tm_hour, timenow->tm_min, timenow->tm_sec, n, pint->num);
        for (int j = 0; j < 3; j++) printf("%c ", pint->buffer[j]);
        printf("\n");
        shmdt(addr);
        sem_op.sem_num = 0;
        sem_op.sem_op = 1;
        sem_op.sem_flg = 0;
        semop(sem_empty, &sem_op, 1);
        sem_op.sem_num = 0;
        sem_op.sem_op = 1;
        sem_op.sem_flg = 0;
        semop(sem_set_id, &sem_op, 1);
    }
    printf("Consumer %d exit\n", n);
    return 0;
}