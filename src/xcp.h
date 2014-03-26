#ifndef XCP_H
#define XCP_H

#include "os.h"
#include "aes.h"
#include <time.h>
#include <errno.h>

#if defined(XCP_WIN)
// in windows system

#include <windows.h>
#include <WinBase.h>
#include <io.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define PATH_DIV '\\'


#else
// in linux system
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#define PATH_DIV '/'

// total size, in bytes
off_t getSize(const char *path);
// protection
mode_t getMode(const char *path);
// time of last status change
time_t getUpdateTime(const char *path);
// create dir with protedction
int createDir(const char *path, mode_t mode);

#endif

int isDir(const char* path);

int xcpFile(const char *srcFile, const char *destPath, int x_kind, const unsigned char *key);
int xcpDir(const char *srcPath, const char *destDir, int x_kind, const unsigned char *key);

#endif