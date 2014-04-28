#include "func.h"

void MD5(const char *str, unsigned char digest[16])
{
	MD5Context context;
	MD5_Init(&context);
	MD5_Update(&context, (unsigned char*)str, strlen(str));
	MD5_Final(&context, digest);
}

int MD5_File(const char *filename, unsigned char digest[16], size_t offset,
	const unsigned char *head, size_t headlen,
	const unsigned char *tail, size_t taillen)
{
	FILE *file;
	MD5Context context;
	size_t readSize;
	unsigned char readBuf[BUF_SIZE];

	if (!(file = fopen(filename, "rb")))
		return RET_ERROR;
	
	MD5_Init(&context);
	
	if (head != NULL)
		MD5_Update(&context, head, headlen);

	fseek(file, offset, SEEK_SET);
	while ((readSize = fread(readBuf, 1, BUF_SIZE, file)) > 0)
		MD5_Update(&context, readBuf, readSize);
	fclose(file);

	if (tail != NULL)
		MD5_Update(&context, tail, taillen);

	MD5_Final(&context, digest);
	
	return RET_YES;
}

void MD5_Str(const unsigned char digest[16], char str[33])
{
	int i = 0, tmp = 0;
	for (i = 0; i < 16; ++i)
	{
		tmp = (digest[i] & 0xF0) >> 4;
		str[2*i] = tmp > 9 ? ('a' + tmp - 10) : ('0' + tmp);
		tmp = digest[i] & 0x0F;
		str[2*i + 1] = tmp > 9 ? ('a' + tmp - 10) : ('0' + tmp);
	}
	str[32] = '\0';
}


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

int deleteFile(const char *path)
{
	TCHAR path_t[PATH_MAX];
	c2t(path, path_t);
	if (DeleteFile(path_t))
		return RET_YES;
	else
		return RET_NO;
}

int X_findfirst(const char *path, X_DIR *xdir)
{
	int len = 0;
	TCHAR tmp[PATH_MAX] = _T("");

	if (xdir == NULL)
		return RET_ERROR;

	c2t(path, tmp);
	len = _tcslen(tmp);
	tmp[len] = tmp[len - 1] == PATH_DIV ? '\0' : PATH_DIV;
	_tcscat(tmp, _T("*"));
	xdir->xfind = FindFirstFile(tmp, &xdir->fd);
	if (xdir->xfind == INVALID_HANDLE_VALUE)
		return RET_END;

	t2c(xdir->fd.cFileName, xdir->name);
	return RET_YES;
}

int X_findnext(X_DIR *xdir)
{
	if (xdir == NULL)
		return RET_ERROR;

	if (!FindNextFile(xdir->xfind, &xdir->fd))
		return RET_END;

	t2c(xdir->fd.cFileName, xdir->name);
	return RET_YES;
}

void X_findclose(X_DIR *xdir)
{
	FindClose(xdir->xfind);
	xdir->xfind = INVALID_HANDLE_VALUE;
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

int deleteFile(const char *path)
{
	if (unlink(path) == 0)
		return RET_YES;
	else
		return RET_NO;
}

int X_findfirst(const char *path, X_DIR *xdir)
{
	if (xdir == NULL)
		return RET_ERROR;

	xdir->xfind = opendir(path);;
	if (!xdir->xfind)
		return RET_END;

	xdir->pdt = readdir(xdir->xfind);
	if (xdir->pdt == NULL)
		return RET_END;

	strcpy(xdir->name, pdt->d_name)
	return RET_YES;
}

int X_findnext(X_DIR *xdir)
{
	if (xdir == NULL)
		return RET_ERROR;

	xdir->pdt = readdir(xdir->xfind);
	if (xdir->pdt == NULL)
		return RET_END;

	strcpy(xdir->name, pdt->d_name)
	return RET_YES;
}

void X_findclose(X_DIR *xdir)
{
	closedir(xdir->xfind);
	xdir->xfind = NULL;
}

#endif