#include "func.h"


#if defined(XCP_WIN)
// in windows system

// get dir or filename(with extname) without PATH_DIV
void getName(const TCHAR *path, TCHAR *name)
{
	int l = 0, r = 0;

	if (path == NULL || name == NULL) return;

	r = _tcslen(path) - 1;
	if (r < 0)
	{
		name[0] = _T('\0');
		return;
	}

	if (path[r] == _T('/')) --r;
	for (l = r - 1; l >= 0 && path[l] != _T('/'); --l);
	if (l < 0 || path[l] == _T('/')) ++l;

	r = r - l;
	_tmemcpy(name, &path[l], r + 1);
	name[r + 1] = '\0';
}


// get extName with '.', get '.' if no extName
void getExtName(const TCHAR *path, TCHAR *extName)
{
	const TCHAR *tmp;

	if (path == NULL || extName == NULL) return;

	tmp = _tcsrchr(path, '.');
	if (tmp == NULL)
		_tcscpy(extName, _T("."));
	else
		_tcscpy(extName, tmp);
}


void c2w(const char *str, wchar_t *wstr)
{
	size_t len = MultiByteToWideChar(CP_ACP, 0, str, -1, 0, 0);;
	MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
	wstr[len] = L'\0';
}


void w2c(const wchar_t *wstr, char *str)
{
	size_t len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_OEMCP, 0, wstr, -1, str, len, NULL, NULL);
	str[len] = '\0';
}


int getFD(const TCHAR *path, WIN32_FIND_DATA *fd)
{
	HANDLE handle = FindFirstFile(path, fd);
	if (handle == INVALID_HANDLE_VALUE)
		return RET_ERROR;
	else
	{
		FindClose(handle);
		return RET_YES;
	}
}


int isDir(const TCHAR* path, const WIN32_FIND_DATA *fd)
{
	if (fd == NULL) return RET_ERROR;
	return fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? RET_YES : RET_NO;
}


// whether a file(normal or readonly or sparse)
int isReg(const TCHAR *path, const WIN32_FIND_DATA *fd)
{
	if (fd == NULL) return RET_ERROR;
	return fd->dwFileAttributes & FILE_ATTRIBUTE_NORMAL
		|| fd->dwFileAttributes & FILE_ATTRIBUTE_READONLY
		|| fd->dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE
		? RET_YES : RET_NO;
}


// time of last update
time_t getUpdateTime(const TCHAR *path, const WIN32_FIND_DATA *fd)
{
	ULARGE_INTEGER ul;
	if (fd == NULL) return RET_ERROR;
		
	ul.LowPart = fd->ftLastWriteTime.dwLowDateTime;
	ul.HighPart = fd->ftLastWriteTime.dwHighDateTime;
	return (long long)(ul.QuadPart - 116444736000000000) / 10000000;
}


// the parent dir must exists
int createDir(const TCHAR *path)
{
	if (CreateDirectory(path, NULL) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS && PathIsDirectory(path))
			return RET_YES;
		else
			return RET_ERROR;
	}
	return RET_YES;
}




#else
// in linux system

// get dir or filename(with extname) without PATH_DIV
void getName(const char *path, char *name)
{
	int l = 0, r = 0;

	if (path == NULL || name == NULL) return;

	r = strlen(path) - 1;
	if (r < 0)
	{
		name[0] = '\0';
		return;
	}

	if (path[r] == '/') --r;
	for (l = r - 1; l >= 0 && path[l] != '/'; --l);
	if (l < 0 || path[l] == '/') ++l;

	r = r - l;
	memcpy(name, &path[l], r + 1);
	name[r + 1] = '\0';
}


// get extName with '.', get '.' if no extName
void getExtName(const char *path, char *extName)
{
	int l = 0, r = 0;

	r = strlen(path) - 1;
	if (path == NULL || extName == NULL) return;
	if (r < 0 || path[r] == '/' || path[r] == '.')
	{
		extName[0] = '.';
		extName[1] = '\0';
		return;
	}

	for (l = r - 1; l >= 0 && path[l] != '.'; --l);
	if (l < 0)
	{
		extName[0] = '.';
		extName[1] = '\0';
		return;
	}
	else
	{
		memcpy(extName, &path[l], r - l + 1);
		extName[r - l + 1] = '\0';
	}
		
}


// whether a dir
int isDir(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return S_ISDIR(st.st_mode & S_IFMT) > 0 ? RET_YES : RET_NO;
}


// whether a regular file
int isReg(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return S_ISREG(st.st_mode & S_IFMT) > 0 ? RET_YES : RET_NO;
}


// file protection
mode_t getMode(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return st.st_mode;
}


// time of last update
time_t getUpdateTime(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return st.st_ctime;
}


// the parent dir must exists
int createDir(const char *path, mode_t mode)
{
	if (mkdir(path, mode) == -1)
	{
		if (errno == EEXIST && isDir(path) == RET_YES)
			return RET_YES;
		else
			return RET_ERROR;
	}
	return RET_YES;
}

#endif