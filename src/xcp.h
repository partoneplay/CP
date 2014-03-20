#ifndef XCP_H
#define XCP_H

#include "os.h"
#include <time.h>

#if defined(XCP_WIN)
#include <windows.h>

// in windows system

#else
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

// linux system
int isDir(const char* path);
// total size, in bytes
off_t getSize(const char *path);
// protection
mode_t getMode(const char *path);
// time of last status change
time_t getCTime(const char *path);
// create dir with protedction
int createDir(const char *path, mode_t mode);

#endif


int xcpFile(const char *srcFile, const char *destPath, int x_kind, const char *key);
int xcpDir(const char *srcPath, const char *destDir, int x_kind, const char *key);


#endif