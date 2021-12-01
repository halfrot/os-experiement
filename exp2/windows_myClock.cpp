#include<Windows.h>
#include<stdio.h>
int main(int argc, char* argv[]) {
	if (argc == 1) {
		printf("-------New process begin-------\n");
		printf("No t parameter\n");
		printf("--------New process end--------\n");
	}
	else {
		printf("-------New process begin-------\n");
		SYSTEMTIME begin_time;
		GetSystemTime(&begin_time);
		printf("Sleep for %s\n", argv[1]);
		// 将参数t转换为int类型
		int sleep_time = 0;
		for (int i = 0; argv[1][i]; i++) {
			sleep_time = sleep_time * 10 + argv[1][i] - '0';
		}
		SYSTEMTIME end_time;
		GetSystemTime(&end_time);
		//int tt = sleep_time - (end_time.wMilliseconds - begin_time.wMilliseconds);
		//printf("%d\n", tt);
		// 减去创建变量花去的时间，减少误差
		Sleep(sleep_time - (end_time.wMilliseconds - begin_time.wMilliseconds));
		printf("--------New process end--------\n");
	}
	return 0;
}