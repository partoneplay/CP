#include "os.h"
#include <time.h>
#include <errno.h>


#if defined(XCP_WIN)
#	define PATH_DIV '\\'
#else
#	define PATH_DIV '/'
#endif


// get dir or filename(with extname) without PATH_DIV
void getName(const char *path, char *name);
// get extName with '.', get '.' if no extName
void getExtName(const char *path, char *extName);
// whether a dir
int isDir(const char *path);
// time of last update
time_t getLastUpdateTime(const char *path);


#if defined(XCP_WIN)
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

// transform between char and TCHAR
void c2t(const char *str, TCHAR *tstr);
void t2c(const TCHAR *tstr, char *str);
// the parent dir must exists
int createDir(const char *path);

#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

// file protection
mode_t getMode(const char *path);
// the parent dir must exists
int createDir(const char *path, mode_t mode);

#endif
