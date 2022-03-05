#include<Windows.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<atlconv.h>
// getCmdLine函数用于解析命令行参数，将多个参数放入字符串cmdLine中，空格隔开
void getCmdLine(char* cmdLine, int argc, char* argv[]) {
	if (argc == 3) {
		int lenArgv1 = strlen(argv[1]);
		int lenArgv2 = strlen(argv[2]);
		for (int i = 0; i < lenArgv1; i++) {
			cmdLine[i] = argv[1][i];
		}
		cmdLine[lenArgv1] = ' ';
		for (int i = 0; i < lenArgv2; i++) {
			cmdLine[lenArgv1 + 1 + i] = argv[2][i];
		}
		cmdLine[lenArgv1 + lenArgv2 + 1] = '\0';
	}
	else if (argc == 2) {
		strcpy(cmdLine, argv[1]);
	}
	else if (argc == 4) {
		int lenArgv1 = strlen(argv[1]);
		int lenArgv2 = strlen(argv[2]);
		int lenArgv3 = strlen(argv[3]);
		for (int i = 0; i < lenArgv1; i++) {
			cmdLine[i] = argv[1][i];
		}
		cmdLine[lenArgv1] = ' ';
		for (int i = 0; i < lenArgv2; i++) {
			cmdLine[lenArgv1 + 1 + i] = argv[2][i];
		}
		cmdLine[lenArgv1 + lenArgv2 + 1] = ' ';
		for (int i = 0; i < lenArgv3; i++) {
			cmdLine[lenArgv1 + lenArgv2 + 2 + i] = argv[3][i];
		}
		cmdLine[lenArgv1 + lenArgv2 + lenArgv3 + 2] = '\0';
	}
}
// calMs函数计算子进程以ms为单位的运行时间
long long calMs(SYSTEMTIME begin_time, SYSTEMTIME end_time) {
	long long ms = end_time.wDay - begin_time.wDay;
	ms = ms * 24 + end_time.wHour - begin_time.wHour;
	ms = ms * 60 + end_time.wMinute - begin_time.wMinute;
	ms = ms * 60 + end_time.wSecond - begin_time.wSecond;
	ms = ms * 1000 + end_time.wMilliseconds - begin_time.wMilliseconds;
	return ms;
}
// 以标准格式输出子进程运行时间
void print(SYSTEMTIME begin_time, SYSTEMTIME end_time) {
	long long tot = calMs(begin_time, end_time);
	int ms = tot % 1000;
	tot = (tot - ms) / 1000;
	int second = tot % 60;
	tot = (tot - second) / 60;
	int minute = tot % 60;
	tot = (tot - minute) / 60;
	int hour = tot;
	printf("%d小时%d分%d秒%d毫秒", hour, minute, second, ms);
}
int main(int argc, char* argv[]) {
	//system(argv[1]);
	//std::cout << argv[0] << std::endl;
	// 定义并初始化STARTUPINFO类型的si
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	// 定义并初始化PROCESS_INFORMATION类型的pi
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	char CharcmdLine[50];
	memset(CharcmdLine, 0, sizeof(CharcmdLine));
	getCmdLine(CharcmdLine, argc, argv);
	printf("%s\n", CharcmdLine);
	SYSTEMTIME begin_time, end_time;
	// 将char类型的CharcmdLine转换为LPWSTR类型
	USES_CONVERSION;
	LPWSTR LPWSTRcmdLine = A2W(CharcmdLine);
	bool bCP = CreateProcess(NULL, LPWSTRcmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	//std::cout << GetLastError() << std::endl;
	GetSystemTime(&begin_time);
	if (!bCP) {
		printf("Create Fail");
		return 0;
	}
	// 等待子进程结束信号
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetSystemTime(&end_time);
	print(begin_time, end_time);
	return 0;
}