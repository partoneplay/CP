#ifndef LINUX_H
#define LINUX_H

#include "global.h"
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

int isDir(const char* path);


// total size, in bytes
off_t getSize(const char *path);
// protection
mode_t getMode(const char *path);
// time of last status change
time_t getCTime(const char *path);


int createDir(const char *path, mode_t mode);
int copyFile(const char *srcFile, const char *destPath, int x_kind);
int copyDir(const char *srcPath, const char *destDir);

#endif