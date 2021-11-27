#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[])
{
    struct timeval begin_time, end_time;
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        printf("Create Fail");
        return 0;
    }
    if (pid == 0)
    {
        int ret = execvp(argv[1], &argv[1]);
        ret = execv(argv[1], &argv[1]);
        if (ret == -1)
        {
            printf("Open Fail");
            return 0;
        }
    }
    else
    {
        gettimeofday(&begin_time, NULL);
        wait(NULL);
        gettimeofday(&end_time, NULL);
        int tot = (end_time.tv_sec - begin_time.tv_sec) * 1000000 + (end_time.tv_usec - begin_time.tv_usec);
        int us = tot % 1000;
        tot = (tot - us) / 1000;
        int ms = tot % 1000;
        tot = (tot - ms) / 1000;
        int s = tot % 60;
        tot = (tot - s) / 60;
        int min = tot % 60;
        tot = (tot - s) / 60;
        int hour = tot;
        printf("%d小时%d分%d秒%d毫秒%d微秒\n", hour, min, s, ms, us);
    }
    return 0;
}