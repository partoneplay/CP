#include "func.h"


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

	if (path[r] == PATH_DIV) --r;
	for (l = r - 1; l >= 0 && path[l] != PATH_DIV; --l);
	if (l < 0 || path[l] == PATH_DIV) ++l;

	r = r - l;
	memcpy(name, &path[l], r + 1);
	name[r + 1] = '\0';
}

void getExtName(const char *path, char *extName)
{
	int l = 0, r = 0;

	r = strlen(path) - 1;
	if (path == NULL || extName == NULL) return;
	if (r < 0 || path[r] == PATH_DIV || path[r] == '.')
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

int isDir(const char *path)
{
#if defined(XCP_WIN)
	TCHAR tpath[PATH_MAX];
	c2t(path, tpath);
	return PathIsDirectory(tpath) ? RET_YES : RET_NO;
#else
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return S_ISDIR(st.st_mode & S_IFMT) > 0 ? RET_YES : RET_NO;
#endif
}

time_t getLastUpdateTime(const char *path)
{
#if defined(XCP_WIN)
	TCHAR tpath[PATH_MAX];
	ULARGE_INTEGER ul;
	WIN32_FIND_DATA fd;

	c2t(path, tpath);
	HANDLE handle = FindFirstFile(tpath, &fd);
	if (handle == INVALID_HANDLE_VALUE) return RET_ERROR;

	ul.LowPart = fd.ftLastWriteTime.dwLowDateTime;
	ul.HighPart = fd.ftLastWriteTime.dwHighDateTime;
	return (long long)(ul.QuadPart - 116444736000000000) / 10000000;
#else
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return st.st_ctime;
#endif
}


#if defined(XCP_WIN)

void c2t(const char *str, TCHAR *tstr)
{
#if defined(UNICODE)
	size_t len = MultiByteToWideChar(CP_ACP, 0, str, -1, 0, 0);;
	MultiByteToWideChar(CP_ACP, 0, str, -1, tstr, len);
	tstr[len] = _T('\0');
#else
	strcpy(tstr, str);
#endif
}

void t2c(const TCHAR *tstr, char *str)
{
#if defined(UNICODE)
	size_t len = WideCharToMultiByte(CP_ACP, 0, tstr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_OEMCP, 0, tstr, -1, str, len, NULL, NULL);
	str[len] = _T('\0');
#else
	strcpy(tstr, str);
#endif
}

int createDir(const char *path)
{
	TCHAR tpath[PATH_MAX] = _T("");
	c2t(path, tpath);
	if (CreateDirectory(tpath, NULL) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS && PathIsDirectory(tpath))
			return RET_YES;
		else
			return RET_ERROR;
	}
	return RET_YES;
}

#else

mode_t getMode(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return st.st_mode;
}

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