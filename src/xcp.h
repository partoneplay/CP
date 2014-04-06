#ifndef XCP_H
#define XCP_H

#include "os.h"


int X_encrypt(const char *srcFile, const char *newFile, const unsigned char *userKey);
int X_decrypt(const char *srcFile, const char *newFile, const unsigned char *userKey);
int X_check(const char *filename, const unsigned char *userKey);
int X_copy(const char *srcFile, const char *newFile);

int xcpFile(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key);
int xcp(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key);
#endif