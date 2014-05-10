#ifndef XCP_H
#define XCP_H

#include "os.h"

int xcpFile(const char *srcPath, const char *destPath, int x_kind, int x_crypt, const unsigned char *key);
int xcp(const char *srcPath, const char *destPath, int x_kind, int x_crypt, const unsigned char *key);

#endif