#include "os.h"
#include "md5.h"

#if defined(XCP_WIN)
#	define PATH_DIV '\\'
#else
#	define PATH_DIV '/'
#endif

// md5sum
void MD5(const char *str, unsigned char digest[16]);
void MD5_Str(const unsigned char digest[16], char str[33]);
int MD5_File(const char *filename, unsigned char digest[16], size_t offset, 
	const unsigned char *head, size_t headlen, 
	const unsigned char *tail, size_t taillen);


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
// delete file
int deleteFile(const char *path);

typedef struct
{
	WIN32_FIND_DATA fd;

	HANDLE xfind;
	char name[NAME_MAX];
} X_DIR;

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
// delete file
int deleteFile(const char *path);

typedef struct
{
	struct dirent *pdt;

	DIR *xfind;
	char name[NAME_MAX];
} X_DIR;

#endif


// Traversing files
int X_findfirst(const char *path, X_DIR *xdir);
int X_findnext(X_DIR *xdir);
void X_findclose(X_DIR *xdir);
