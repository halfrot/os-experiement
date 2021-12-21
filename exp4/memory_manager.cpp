#include<Windows.h>
#include<iostream>
#include<iomanip>
#include<string>
#include<Tlhelp32.h>
#include<stdio.h>
#include<tchar.h>
#include<shlwapi.h>
#include<psapi.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib,"kernel32.lib")
using namespace std;
void displaySystemConfig() {
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));
	GetSystemInfo(&si);
	TCHAR str_page_size[MAX_PATH];
	StrFormatByteSize(si.dwPageSize, str_page_size, MAX_PATH);
	DWORD memory_size = (DWORD)si.lpMaximumApplicationAddress - (DWORD)si.lpMinimumApplicationAddress;
	TCHAR str_memory_size[MAX_PATH];
	StrFormatByteSize(memory_size, str_memory_size, MAX_PATH);
	TCHAR str_Granularity[MAX_PATH];
	StrFormatByteSize(si.dwAllocationGranularity, str_Granularity, MAX_PATH);
	cout << "GetSystemInfo" << endl;
	cout << "--------------------------------------------" << endl;
	int wProcessorArchitecture = si.wProcessorArchitecture;
	cout << "Process architect              | ";
	if (wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) cout << "x64";
	else if (wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) cout << "x86";
	else cout << "?";
	cout << endl;
	cout << "Process Core                   | " << si.dwNumberOfProcessors << endl;
	cout << "Virtual memory page size       | " << str_page_size << endl;
	cout << "Minimum application address    | 0x" << hex << setfill('0') << setw(8) << (DWORD)si.lpMinimumApplicationAddress << endl;
	cout << "Maximum application address    | 0x" << hex << setw(8) << (DWORD)si.lpMaximumApplicationAddress << endl;
	cout << "Total available virtual memory | " << str_memory_size << endl;
	cout << "CPU level                      | " << setbase(10) << si.wProcessorLevel << endl;
	cout << "dwAllocationGranularity        | " << str_Granularity << endl;
	cout << "--------------------------------------------" << endl;
}
void displayMemoryCondition() {
	printf("GlobalMemoryStatusEx\n");
	cout << "--------------------------------------------" << endl;
	MEMORYSTATUSEX stat;
	stat.dwLength = sizeof(stat);
	GlobalMemoryStatusEx(&stat);
	double DIV = (long long)1024 * 1024 * 1024;
	cout << "Memory Load               | " << setbase(10) << stat.dwMemoryLoad << "%\n"
		<< "Total physical memory     | " << stat.ullTotalPhys / DIV << "GB\n"
		<< "Available physical memory | " << stat.ullAvailPhys / DIV << "GB\n"
		<< "Total page file           | " << stat.ullTotalPageFile / DIV << "GB\n"
		<< "Avaliable page file       | " << stat.ullAvailPageFile / DIV << "GB\n"
		<< "Total virtual memory      | " << stat.ullTotalVirtual / DIV << "GB\n"
		<< "Avaliable virtual memory  | " << stat.ullAvailVirtual / DIV << "GB" << endl;
	cout << "--------------------------------------------" << endl;
}
void displaySystemPerformance() {
	cout << "SystemPerformanceInfo" << endl;
	cout << "--------------------------------------------" << endl;
	PERFORMANCE_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	pi.cb = sizeof(pi);
	GetPerformanceInfo(&pi, pi.cb);
	long long DIV = 1024;
	cout << "Handle count              | " << setbase(10) << pi.HandleCount << "\n"
		<< "Process count             | " << pi.ProcessCount << "\n"
		<< "Page Size                 | " << pi.PageSize / DIV << "KB\n"
		<< "Physical total page       | " << pi.PhysicalTotal << "\n"
		<< "System cache page         | " << pi.SystemCache << endl;
	cout << "--------------------------------------------" << endl;
}
void getAllProcessInformation() {
	printf("CreateToolhelp32Snapshot\n");
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessShot == INVALID_HANDLE_VALUE) {
		printf("CreateToolhelp32Snapshot error.\n");
		return;
	}
	cout << " |  num  |  pid  |  ProcessName" << endl;
	cout << "-----------------------------------------" << endl;
	if (Process32First(hProcessShot, &pe32))
		for (int i = 0; Process32Next(hProcessShot, &pe32); i++)
			printf(" | %4d  | %5d  |  %s\n", i, pe32.th32ProcessID, pe32.szExeFile);
	cout << "-----------------------------------------" << endl;
	CloseHandle(hProcessShot);
}
void printProtection(DWORD dwTarget) {
	char as[] = "----------";
	if (dwTarget & PAGE_NOACCESS) as[0] = 'N';
	if (dwTarget & PAGE_READONLY) as[1] = 'R';
	if (dwTarget & PAGE_READWRITE) as[2] = 'W';
	if (dwTarget & PAGE_WRITECOPY) as[3] = 'C';
	if (dwTarget & PAGE_EXECUTE) as[4] = 'X';
	if (dwTarget & PAGE_EXECUTE_READ) as[5] = 'r';
	if (dwTarget & PAGE_EXECUTE_READWRITE) as[6] = 'w';
	if (dwTarget & PAGE_EXECUTE_WRITECOPY) as[7] = 'c';
	if (dwTarget & PAGE_GUARD) as[8] = 'G';
	if (dwTarget & PAGE_NOCACHE) as[9] = 'D';
	if (dwTarget & PAGE_WRITECOMBINE) as[10] = 'B';
	printf("  %s  ", as);
}
void getProcessDetail(int pid) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	if (!hProcess) {
		cout << "OpenProcess Fail" << endl;
		return;
	}
	cout << " | "
		<< "   Memory Addr    | "
		<< "   Size    | "
		<< "PageStatus| "
		<< "    Protect    | "
		<< "  Type  | "
		<< " ModuleName"
		<< endl;
	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(si));
	GetSystemInfo(&si);
	MEMORY_BASIC_INFORMATION mbi;
	ZeroMemory(&mbi, sizeof(mbi));
	LPCVOID pBlock = (LPVOID)si.lpMinimumApplicationAddress;
	while (pBlock < si.lpMaximumApplicationAddress && VirtualQueryEx(hProcess, pBlock, &mbi, sizeof(mbi)) == sizeof(mbi)) {
		LPCVOID pEnd = (PBYTE)pBlock + mbi.RegionSize;
		if (pEnd >= si.lpMaximumApplicationAddress) break;
		TCHAR szSize[MAX_PATH];
		StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);
		cout << " | ";
		cout.fill('0');
		cout << hex << setw(8) << (LPCVOID)pBlock
			<< "-"
			<< hex << setw(8) << (LPCVOID)pEnd
			<< " | ";
		printf("%10s", szSize);
		switch (mbi.State) {
		case MEM_COMMIT:cout << " | " << setw(9) << "Committed" << " | "; break;
		case MEM_FREE:cout << " | " << setw(9) << "   Free  " << " | "; break;
		case MEM_RESERVE:cout << " | " << setw(9) << " Reserved" << " | "; break;
		default: cout << "          | "; break;
		}
		if (mbi.Protect == 0 && mbi.State != MEM_FREE) mbi.Protect = PAGE_READONLY;
		printProtection(mbi.Protect);
		switch (mbi.Type) {
		case MEM_IMAGE:cout << " |  Image  | "; break;
		case MEM_PRIVATE:cout << " | Private | "; break;
		case MEM_MAPPED:cout << " |  Mapped | "; break;
		default:cout << " |         | "; break;
		}
		TCHAR str_module_name[MAX_PATH];
		if (GetModuleFileName((HMODULE)pBlock, str_module_name, MAX_PATH) > 0) {
			PathStripPath(str_module_name);
			printf("%s", str_module_name);
		}
		cout << endl;
		pBlock = pEnd;
	}
}
void ShowHelp() {
	cout << "--------------------------------------------------------------------------" << endl;
	cout << "Type command: " << endl
		<< "[config]     : Display system memory configuration." << endl
		<< "[condition]  : Display system memory usage condition." << endl
		<< "[performance]: Display system memory usage condition." << endl
		<< "[process]    : Display all running processes information (with pid)." << endl
		<< "[pid]        : Type a pid number and display detail of process with pid" << endl
		<< "[help]       : Display this help text." << endl
		<< "[exit]       : Exit" << endl;
	cout << "--------------------------------------------------------------------------" << endl;
}
int main() {
	cout << "-----------System Memory Manager-----------" << endl << endl;
	cout << "Type [help] for help.\n" << endl;
	string cmd;
	char cmd_charstr[127];
	while (true) {
		cout << "Manager> ";
		cin.getline(cmd_charstr, 127);
		cmd = cmd_charstr;
		cout << endl;
		if (cmd == "") continue;
		if (cmd == "config") displaySystemConfig();
		else if (cmd == "condition") displayMemoryCondition();
		else if (cmd == "performance") displaySystemPerformance();
		else if (cmd == "process") getAllProcessInformation();
		else if (cmd == "pid") {
			cout << "PID> ";
			int pid = 0;
			cin >> pid;
			cin.getline(cmd_charstr, 127);
			if (pid <= 0) continue;
			cout << endl;
			getProcessDetail(pid);
		}
		else if (cmd == "help") ShowHelp();
		else if (cmd == "exit") return 0;
		else if (cmd == "clear" || cmd == "cls") system("cls");
		else {
			cout << "Invalid command" << endl;
			cout << "Type [help] for help.\n" << endl;
			fflush(stdin);
			cin.clear();
		}
	}
	return 0;
}
