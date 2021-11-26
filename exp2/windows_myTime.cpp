#include<Windows.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<atlconv.h>
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
	else {
		strcpy(cmdLine, argv[1]);
	}
}
long long calMs(SYSTEMTIME begin_time, SYSTEMTIME end_time) {
	long long ms = end_time.wDay - begin_time.wDay;
	ms = ms * 24 + end_time.wHour - begin_time.wHour;
	ms = ms * 60 + end_time.wMinute - begin_time.wMinute;
	ms = ms * 60 + end_time.wSecond - begin_time.wSecond;
	ms = ms * 1000 + end_time.wMilliseconds - begin_time.wMilliseconds;
	return ms;
}
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
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	char CharcmdLine[20];
	getCmdLine(CharcmdLine, argc, argv);
	//printf("%s\n", CharcmdLine);
	SYSTEMTIME begin_time, end_time;
	USES_CONVERSION;
	LPWSTR LPWSTRcmdLine = A2W(CharcmdLine);
	bool bCP = CreateProcess(NULL, LPWSTRcmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	//std::cout << GetLastError() << std::endl;
	GetSystemTime(&begin_time);
	if (!bCP) {
		printf("Create Fail");
		return 0;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetSystemTime(&end_time);
	print(begin_time, end_time);
	return 0;
}