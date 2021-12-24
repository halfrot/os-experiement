#include <cstdio>
#include <cstring>
#include <algorithm>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <utime.h>
using namespace std;
const int BUFFSIZE = 1 << 10;
void copyAttribute(string source, string target)
{
    struct stat StatBuf;
    lstat(source.c_str(), &StatBuf);
    chmod(target.c_str(), StatBuf.st_mode);
    chown(target.c_str(), StatBuf.st_uid, StatBuf.st_gid);
    if (S_ISLNK(StatBuf.st_mode))
    {
        struct timeval TimeSource[2];
        TimeSource[0].tv_sec = StatBuf.st_atime;
        TimeSource[0].tv_usec = 0;
        TimeSource[1].tv_sec = StatBuf.st_mtime;
        TimeSource[1].tv_usec = 0;
        lutimes(target.c_str(), TimeSource);
    }
    else
    {
        struct utimbuf TimeSource;
        TimeSource.actime = StatBuf.st_atime;
        TimeSource.modtime = StatBuf.st_mtime;
        utime(target.c_str(), &TimeSource);
    }
}
void copyNormalFile(string source, string target)
{
    int FCBSource = open(source.c_str(), O_RDONLY);
    int FCBTarget = creat(target.c_str(), O_WRONLY);
    char buf[BUFFSIZE];
    int tot = 0;
    while ((tot = read(FCBSource, buf, BUFFSIZE)) > 0)
        write(FCBTarget, buf, tot);
    close(FCBSource);
    close(FCBTarget);
    copyAttribute(source, target);
}
void copyLink(string source, string target)
{
    string real(realpath(source.c_str(), NULL));
    symlink(real.c_str(), target.c_str());
    copyAttribute(source, target);
}
void copyDir(string source, string target)
{
    DIR *PDSource = opendir(source.c_str());
    struct dirent *EntrySource = NULL;
    struct stat StatBuf;
    lstat(source.c_str(), &StatBuf);
    if (opendir(target.c_str()) == NULL)
        mkdir(target.c_str(), StatBuf.st_mode);
    while (EntrySource = readdir(PDSource))
    {
        if (string(EntrySource->d_name) == ".." || string(EntrySource->d_name) == ".")
            continue;
        cout << "-----copy " + string(EntrySource->d_name) + "-----" << endl;
        string NextSource = source + "/" + EntrySource->d_name;
        string NextTarget = target + "/" + EntrySource->d_name;
        lstat(NextSource.c_str(), &StatBuf);
        if (S_ISDIR(StatBuf.st_mode))
            copyDir(NextSource, NextTarget);
        else if (S_ISREG(StatBuf.st_mode))
            copyNormalFile(NextSource, NextTarget);
        else if (S_ISLNK(StatBuf.st_mode))
            copyLink(NextSource, NextTarget);
    }
    cout << "*****" << endl
         << "directory " + source + " end" << endl
         << "*****" << endl;
    copyAttribute(source, target);
    closedir(PDSource);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "-----argument error-----" << endl;
        return 0;
    }
    struct stat StatBuf;
    string source(argv[1]), target(argv[2]);
    if (lstat(argv[1], &StatBuf) == -1)
    {
        cout << "-----source dir error-----" << endl;
        return 0;
    }
    if (S_ISDIR(StatBuf.st_mode))
    {
        if (opendir(argv[1]) == NULL)
        {
            cout << "-----source dir error-----" << endl;
            return 0;
        }
        copyDir(source, target);
    }
    return 0;
}