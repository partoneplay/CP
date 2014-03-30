#include "os.h"
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

// get dir or filename(with extname) without PATH_DIV
void getName(const TCHAR *path, TCHAR *name);
// get extName with '.', get '.' if no extName
void getExtName(const TCHAR *path, TCHAR *extName);
// transform between char and TCHAR
void c2t(const char *str, TCHAR *tstr);
void t2c(const TCHAR *tstr, char *str);
// Get FindFileData
int getFD(const TCHAR *path, WIN32_FIND_DATA *fd);
// whether a dir
int isDir(const WIN32_FIND_DATA *fd);
// whether a file(normal or readonly or sparse)
int isReg(const WIN32_FIND_DATA *fd);
// time of last update
time_t getUpdateTime(const WIN32_FIND_DATA *fd);
// the parent dir must exists
int createDir(const TCHAR *path);


#else
// in linux system
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

// get dir or filename(with extname) without PATH_DIV
void getName(const char *path, char *name);
// get extName with '.', get '.' if no extName
void getExtName(const char *path, char *extName);
// whether a dir
int isDir(const char *path);
// whether a regular file
int isReg(const char *path);
// file protection
mode_t getMode(const char *path);
// time of last update
time_t getUpdateTime(const char *path);
// the parent dir must exists
int createDir(const char *path, mode_t mode);

#endif
