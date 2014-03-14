#ifndef LINUX_H
#define LINUX_H

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define BUF_SIZE 1024
#define MAX_LEN 256

#define EXT ".cxc"

/* @return 
 * positive : dir
 * zero : not dir
 * negative : fail
 */
int isDir(const char* path);


// total size, in bytes
off_t getSize(const char *path);
// protection
mode_t getMode(const char *path);
// time of last status change
time_t getCTime(const char *path);


/* @return
 * 0 : dir aleady exists or create successfully
 * -1 : error occurs
 */
int createDir(const char* path, mode_t mode);


/* @return
 * 0 : success
 * -1 : error occurs
 */
int copyFile(const char* filename, const char* destPath);


/* @return
 * 0 : success
 * -1 : error occurs
 */
int copyDir(const char* srcDir, const char* destDir);




//int mapFile(const char *filename);

//int unmapFile();

#endif

// 多线程实现文件拷贝，对比与单线程的效率优劣

// 流水线的可能性实现

// 测试：小文件内存映射的效率与普通方法的拷贝效率比较；多个小文件内存映射的效率；
