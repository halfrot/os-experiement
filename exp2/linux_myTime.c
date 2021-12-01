#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[])
{
    // 记录时间戳的结构体timeval
    struct timeval begin_time, end_time;
    // 保存当前进程pid
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        printf("Create Fail");
        return 0;
    }
    if (pid == 0)
    {
        // execvp和execv调用程序失败时，返回值均为-1
        // 调用成功即进入被调用程序
        int ret = execvp(argv[1], &argv[1]);
        ret = execv(argv[1], &argv[1]);
        // 调用失败
        if (ret == -1)
        {
            printf("Open Fail");
            return 0;
        }
    }
    else
    {
        // 获取开始时间戳
        gettimeofday(&begin_time, NULL);
        // 等待子进程结束
        wait(NULL);
        // 获取结束时间戳
        gettimeofday(&end_time, NULL);
        // 格式化子进程运行时间
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