#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[])
{
    // printf("son argv %s\n",argv[1]);
    if (argc == 1)
    {
        printf("-------New process begin-------\n");
        printf("No t parameter\n");
        printf("--------New process end--------\n");
    }
    else
    {
        printf("-------New process begin-------\n");
        printf("Sleep for %s\n", argv[1]);
        int sleep_time = 0;
        // 将参数t转化为int类型
        for (int i = 0; argv[1][i]; i++)
        {
            sleep_time = sleep_time * 10 + argv[1][i] - '0';
        }
        usleep(sleep_time);
        printf("--------New process end--------\n");
    }
    return 0;
}