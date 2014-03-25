#ifndef XCP_H
#define XCP_H

#include "func.h"
#include "aes.h"
#include <time.h>
#include <errno.h>


#if defined(XCP_WIN)
int xcpFile(const TCHAR *srcPath, const TCHAR *destPath, int x_kind, const unsigned char *key);
int xcp(const TCHAR *srcPath, const TCHAR *destPath, int x_kind, const unsigned char *key);
#else
int xcpFile(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key);
int xcp(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key);
#endif


#endif