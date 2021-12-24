#include<cstdio>
#include<cstring>
#include<algorithm>
#include<Windows.h>
#include<string>
#include<iostream>
using namespace std;
const int BUFFSIZE = 1 << 10;
void copyAttribute(string source, string target) {
	DWORD SourceFileAttribute = GetFileAttributes(source.c_str());
	SetFileAttributes(target.c_str(), SourceFileAttribute);
	HANDLE HSource = CreateFile(source.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	HANDLE HTarget = CreateFile(target.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	FILETIME SourceFileCreate, SourceFileAccess, SourceFileLastWrite;
	GetFileTime(HSource, &SourceFileCreate, &SourceFileAccess, &SourceFileLastWrite);
	SetFileTime(HTarget, &SourceFileCreate, &SourceFileAccess, &SourceFileLastWrite);
}
void copyNormalFile(string source, string target) {
	HANDLE HSource = CreateFile(source.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	HANDLE HTarget = CreateFile(target.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (HTarget == INVALID_HANDLE_VALUE) {
		cout << "-----target file error-----" << endl;
		return;
	}
	char buf[BUFFSIZE];
	DWORD tot = BUFFSIZE;
	while (tot == BUFFSIZE && ReadFile(HSource, buf, BUFFSIZE, &tot, NULL)) WriteFile(HTarget, buf, tot, &tot, NULL);
	CloseHandle(HSource);
	CloseHandle(HTarget);
	copyAttribute(source, target);
}
void copyDir(string source, string target) {
	WIN32_FIND_DATA FindData;
	if (FindFirstFile((target + "\\*.*").c_str(), &FindData) == INVALID_HANDLE_VALUE) CreateDirectory(target.c_str(), NULL);
	HANDLE HFile = FindFirstFile((source + "\\*.*").c_str(), &FindData);
	while (FindNextFile(HFile, &FindData)) {
		if (string(FindData.cFileName) == ".." || string(FindData.cFileName) == ".") continue;
		cout << "-----copy " + string(FindData.cFileName) + "-----" << endl;
		string NextSource = source + "\\" + FindData.cFileName;
		string NextTarget = target + "\\" + FindData.cFileName;
		if (FindData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) copyDir(NextSource, NextTarget);
		else copyNormalFile(NextSource, NextTarget);
	}
	FindClose(HFile);
	cout << "*****" << endl
		<< "directory " + source + " end" << endl
		<< "*****" << endl;
	copyAttribute(source, target);
}
int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "-----argument error-----" << endl;
		return 0;
	}
	WIN32_FIND_DATA FindData;
	string source(argv[1]), target(argv[2]);
	if (FindFirstFile((source + "\\*.*").c_str(), &FindData) == INVALID_HANDLE_VALUE) {
		cout << "-----source dir error-----" << endl;
		return 0;
	}
	if (FindData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) copyDir(source, target);
	else cout << "-----source dir error-----" << endl;
	return 0;
}