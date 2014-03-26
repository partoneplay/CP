#ifndef XCP_H
#define XCP_H

#include "os.h"
#include "aes.h"
#include <time.h>
#include <errno.h>


#if defined(XCP_WIN)
#	define PATH_DIV '\\'
#else
#	define PATH_DIV '/'
#endif


#if defined(XCP_WIN)
// in windows system
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#else
// in linux system
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#endif


#if defined(XCP_WIN)
int xcpFile(const TCHAR *srcPath, const TCHAR *destPath, int x_kind, const unsigned char *key);
int xcp(const TCHAR *srcPath, const TCHAR *destPath, int x_kind, const unsigned char *key);
#else
int xcpFile(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key);
int xcp(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key);
#endif


#endif