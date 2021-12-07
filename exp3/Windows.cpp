#include<stdio.h>
#include<string.h>
#include<Windows.h>
#include<time.h>
#define PNUM 2
#define CNUM 3
struct myBuf {
	int num;
	int read;
	int write;
	char buffer[5];
};
BOOL StartClone(HANDLE* lpHandle) {
	BOOL bCreateOK;
	PROCESS_INFORMATION pi;
	TCHAR szFilename[MAX_PATH];
	GetModuleFileName(NULL, szFilename, MAX_PATH);
	TCHAR szCmdLine[MAX_PATH];
	int temp_cnt = 0;
	for (int i = 0; i < 3; i++) {
		sprintf(szCmdLine, "\"%s\" consumer %d", szFilename, i);
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		bCreateOK = CreateProcess(szFilename, szCmdLine, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi);
		if (!bCreateOK) return false;
		lpHandle[temp_cnt] = pi.hProcess;
		temp_cnt++;
	}
	for (int i = 0; i < 2; i++) {
		sprintf(szCmdLine, "\"%s\" productor %d", szFilename, i);
		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		bCreateOK = CreateProcess(szFilename, szCmdLine, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &si, &pi);
		if (!bCreateOK) return false;
		lpHandle[temp_cnt] = pi.hProcess;
		temp_cnt++;
	}
	return true;
}
void Parent() {
	printf("Creating the child process and waited child process to quit.\n");
	HANDLE hMutexMapping = CreateMutex(NULL, true, "mutex");
	HANDLE hMapping = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, 10, "map");
	if (hMapping != INVALID_HANDLE_VALUE) {
		LPVOID pData = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pData != NULL) ZeroMemory(pData, 10);
		struct myBuf* pnData = reinterpret_cast<struct myBuf*>(pData);
		pnData->read = 0;
		pnData->write = 0;
		pnData->num = 0;
		memset(pnData->buffer, 0, sizeof(pnData->buffer));
		UnmapViewOfFile(pData);
	}
	CreateSemaphore(NULL, 3, 3, "EMPTY");
	CreateSemaphore(NULL, 0, 3, "FULL");
	HANDLE lpHandle[10];
	BOOL bCreateOK = StartClone(lpHandle);
	ReleaseMutex(hMutexMapping);
	WaitForMultipleObjects(PNUM + CNUM, lpHandle, true, INFINITE);
	CloseHandle(hMapping);
	CloseHandle(hMutexMapping);
}
void Productor(int n) {
	printf("Productor: %d is running.\n", n);
	HANDLE hMutexMapping = OpenMutex(MUTEX_ALL_ACCESS, true, "mutex");
	HANDLE hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "map");
	if (hMapping == INVALID_HANDLE_VALUE) printf("error\n");
	HANDLE semEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
	HANDLE semFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL");
	SYSTEMTIME st;
	srand((unsigned)time(0));
	for (int i = 0; i < 6; i++) {
		WaitForSingleObject(semEmpty, INFINITE);
		Sleep(rand() % 3000 + 1000);
		WaitForSingleObject(hMutexMapping, INFINITE);
		LPVOID pFile = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pFile != NULL) {
			struct myBuf* pnData = reinterpret_cast<struct myBuf*>(pFile);
			pnData->buffer[pnData->write] = rand() % 26 + 'A';
			pnData->write = (pnData->write + 1) % 3;
			pnData->num++;
			GetLocalTime(&st);
			printf("%02d:%02d:%02d productor[%d] s 缓冲区剩余: %d个 ", st.wHour, st.wMinute, st.wSecond, n, pnData->num);
			for (int j = 0; j < 3; j++) printf("%c ", pnData->buffer[j]);
			printf("\n");
		}
		UnmapViewOfFile(pFile);
		pFile = NULL;
		ReleaseSemaphore(semFull, 1, NULL);
		ReleaseMutex(hMutexMapping);
	}
	printf("-----productor[%d] exit-----\n", n);
}
void Consumer(int n) {
	printf("Consumer: %d is running.\n", n);
	HANDLE hMutexMapping = OpenMutex(MUTEX_ALL_ACCESS, true, "mutex");
	HANDLE hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "map");
	if (hMapping == INVALID_HANDLE_VALUE) printf("error\n");
	HANDLE semEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
	HANDLE semFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL");
	SYSTEMTIME st;
	srand((unsigned)time(0));
	for (int i = 0; i < 4; i++) {
		WaitForSingleObject(semFull, INFINITE);
		Sleep(rand() % 3000 + 1000);
		WaitForSingleObject(hMutexMapping, INFINITE);
		LPVOID pFile = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (pFile != NULL) {
			struct myBuf* pnData = reinterpret_cast<struct myBuf*>(pFile);
			pnData->buffer[pnData->read] = '#';
			pnData->read = (pnData->read + 1) % 3;
			pnData->num--;
			GetLocalTime(&st);
			printf("%02d:%02d:%02d consumer[%d] s 缓冲区剩余: %d个 ", st.wHour, st.wMinute, st.wSecond, n, pnData->num);
			for (int j = 0; j < 3; j++) printf("%c ", pnData->buffer[j]);
			printf("\n");
		}
		UnmapViewOfFile(pFile);
		pFile = NULL;
		ReleaseSemaphore(semEmpty, 1, NULL);
		ReleaseMutex(hMutexMapping);
	}
	printf("-----consumer[%d] exit-----\n", n);
}
int main(int argc, char* argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "productor") == 0) Productor(atoi(argv[2]));
		else if (strcmp(argv[1], "consumer") == 0) Consumer(atoi(argv[2]));
	}
	else Parent();
	return 0;
}