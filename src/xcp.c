#include "xcp.h"




#if defined(XCP_WIN)
#define F_OK 0
#define access _access


#else

#endif


int xcpFile(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key)
{
	size_t len;
	time_t ct_src, ct_new;
	char fileName[NAME_MAX] = "", extName[NAME_MAX] = "", newFile[PATH_MAX] = "";
	char *tmp, md5[33];
	unsigned char digest[16];

	// srcpath must be a regular file
	if (isReg(srcPath) != RET_YES)
		return RET_ERROR;
	getExtName(srcPath, extName);
	getName(srcPath, fileName);

	// generate the srcfile's md5sum
	if (x_kind & X_MD5SUM)
	{
		MD5_File(srcPath, digest, 0, NULL);
		MD5_Str(digest, md5);
		printf("%s    %s\n", md5, srcPath);
		return RET_YES;
	}

	// check the encrypted file
	if (x_kind & X_CHECK)
	{
		// skip the non-encrypt files
		if (strcmp(extName, EXT_NAME) != 0)
			return RET_SKIP;

		if (X_check(srcPath, key) == RET_YES)
			printf("Check OK.    %s\n", srcPath);
		else
			printf("Check Fail.  %s\n", srcPath);
		return RET_YES;
	}


	len = strlen(destPath);
	memcpy(newFile, destPath, len);
	if (isDir(destPath) == RET_YES)
	{
		// the dir must exists
		if (access(destPath, F_OK) == -1)
			return RET_ERROR;
		else
		{
			if (newFile[len - 1] != PATH_DIV)
				newFile[len] = PATH_DIV;
			strcat(newFile, fileName);
		}
	}

	// the encrypted file should have a EXT_NAME
	if (x_kind & X_ENCRYPT)
	{
		tmp = strrchr(newFile, '.');
		if (tmp == NULL || strcmp(tmp, EXT_NAME) != 0)
			strcat(newFile, EXT_NAME);
	}
	else if (x_kind & X_DECRYPT)
	{
		len = strlen(EXT_NAME);
		if (strlen(newFile) <= len)
			return RET_SKIP;
		memset(&newFile[strlen(newFile) - len], 0, len);
	}

	// prevent copy self
	if (strcmp(srcPath, newFile) == 0)
	{
		fprintf(stderr, "Warning : Try to copy self '%s'\n", srcPath);
		return RET_SKIP;
	}

	// update copy
	if (x_kind & X_UPDATE)
	{
		ct_src = getUpdateTime(srcPath);
		ct_new = getUpdateTime(newFile);
		if (ct_new >= ct_src)
		{
			printf("Skip  '%s'\n", srcPath);
			return RET_SKIP;
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		printf("Encrypt '%s' to '%s'\n", srcPath, newFile);
		X_encrypt(srcPath, newFile, key);
	}
	else if (x_kind & X_DECRYPT)
	{
		// skip the non-encrypt files
		if (strcmp(extName, EXT_NAME) != 0)
			return RET_SKIP;

		printf("Decrypt '%s' to '%s'\n", srcPath, newFile);
		X_decrypt(srcPath, newFile, key);
	}
	else
	{
		printf("Copy '%s' to '%s'\n", srcPath, newFile);
		X_copy(srcPath, newFile);
	}

#if defined(XCP_LINUX)
	// update the file protected mode
	chmod(newFile, getMode(srcPath));
#elif defined(XCP_WIN)

#endif

	return RET_YES;
}


#if defined(XCP_WIN)
int xcpDir(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key)
{
	WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
	int ret = 0;
	size_t len = 0;
	char dirName[NAME_MAX] = "";
	char srcBase[PATH_MAX] = "", destBase[PATH_MAX] = "";
#if defined(UNICODE)
	wchar_t dir[PATH_MAX] = L"";
#else
	char dir[PATH_MAX] = "";
#endif

	ret = isDir(srcPath);
	if (ret == RET_NO)
		return xcpFile(srcPath, destPath, x_kind, key);	
	else if (ret == RET_ERROR)
		return RET_ERROR;
	getName(srcPath, dirName);

	len = strlen(srcPath);
	strcpy(srcBase, srcPath);
	srcBase[len] = srcPath[len - 1] == PATH_DIV ? '\0' : PATH_DIV;

	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	{
		while ((hFind = FindFirstFile(dirSpec, &FindFileData)) != NULL)
		{
			memset(srcFile, 0, PATH_MAX);	
			strcpy(srcFile, srcBase);
			strcat(srcFile, pdt->d_name);
		
			ret = isDir(srcFile);
			if (ret == RET_YES)
			{
				if (strcmp(strrchr(srcFile, '/'), "/.") != 0 && strcmp(strrchr(srcFile, '/'), "/..") != 0)
					xcpDir(srcFile, NULL, x_kind, key);
			}
			else if (ret == RET_NO)
				xcpFile(srcFile, NULL, x_kind, key);
		}
		closedir(dir);

		return RET_YES;
	}

	
    char DirSpec[MAX_PATH + 1];// 指定路径
    DWORD dwError;

    strncpy (DirSpec, srcPath, strlen(srcPath) + 1);
    strncat (DirSpec, "/*", 3);

    hFind = FindFirstFile((DirSpec, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf ("Invalid file handle. Error is %u ", GetLastError());
        return ;
    }
    else
    {
        if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
        {
            printf ("　　%s ", FindFileData.cFileName);   //找到文件
        }
        else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
            && strcmp(FindFileData.cFileName, ".") != 0
            && strcmp(FindFileData.cFileName, "..") != 0)
        {   //找到目录
            char Dir[MAX_PATH + 1];
            strcpy(Dir, srcPath);
            strncat(Dir, "/", 2);
            strcat(Dir, FindFileData.cFileName);

            FindFile(Dir);
        }

        while (FindNextFile(hFind, &FindFileData) != 0)
        {
            if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {   //找到文件
                printf ("　　%s ", FindFileData.cFileName);
            }
            else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
                && strcmp(FindFileData.cFileName, ".") != 0
                && strcmp(FindFileData.cFileName, "..") != 0)
            { //找到目录
                char Dir[MAX_PATH + 1];
                strcpy(Dir, srcPath);
                strncat(Dir, "/", 2);
                strcat(Dir, FindFileData.cFileName);
                FindFile(Dir);
            }

        }

        dwError = GetLastError();
        FindClose(hFind);
        if (dwError != ERROR_NO_MORE_FILES)
        {
            printf ("FindNextFile error. Error is %u ", dwError);
            return;
        }
    }

	return xcpFile(srcPath, destDir, x_kind, key);
}


#else
int xcpDir(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key)
{
	DIR *dir;
	int ret;
	size_t len = 0;
	struct dirent *pdt;
	char dirName[NAME_MAX] = "";
	char srcFile[PATH_MAX] = "", destFile[PATH_MAX] = "";
	char srcBase[PATH_MAX] = "", destBase[PATH_MAX] = "";
	
	ret = isDir(srcPath);
	if (ret == RET_NO)
		return xcpFile(srcPath, destPath, x_kind, key);	
	else if (ret == RET_ERROR)
		return RET_ERROR;
	dir = opendir(srcPath);
	getName(srcPath, dirName);

	len = strlen(srcPath);
	strcpy(srcBase, srcPath);
	srcBase[len] = srcPath[len - 1] == PATH_DIV ? '\0' : PATH_DIV;

	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	{
		while ((pdt = readdir(dir)) != NULL)
		{
			memset(srcFile, 0, PATH_MAX);	
			strcpy(srcFile, srcBase);
			strcat(srcFile, pdt->d_name);
		
			ret = isDir(srcFile);
			if (ret == RET_YES)
			{
				if (strcmp(strrchr(srcFile, '/'), "/.") != 0 && strcmp(strrchr(srcFile, '/'), "/..") != 0)
					xcpDir(srcFile, NULL, x_kind, key);
			}
			else if (ret == RET_NO)
				xcpFile(srcFile, NULL, x_kind, key);
		}
		closedir(dir);

		return RET_YES;
	}
		
	len = strlen(destPath);
	strcpy(destBase, destPath);
	destBase[len] = destBase[len - 1] == PATH_DIV ? '\0' : PATH_DIV;
	if (access(destBase, F_OK) == 0)
	{
		strcat(destBase, dirName);
		strcat(destBase, "/");
	}
	if (createDir(destBase, getMode(srcPath)) == RET_ERROR)
	{
		fprintf(stderr, "Fail to Create Dir '%s' : %s\n", destPath, strerror(errno));
		return RET_ERROR;
	}
	
	while ((pdt = readdir(dir)) != NULL)
	{
		memset(srcFile, 0, PATH_MAX);
		memset(destFile, 0, PATH_MAX);

		strcpy(srcFile, srcBase);
		strcat(srcFile, pdt->d_name);
		strcpy(destFile, destBase);
		strcat(destFile, pdt->d_name);

		ret = isDir(srcFile);
		if (ret == RET_YES)
		{
			if (strcmp(strrchr(srcFile, '/'), "/.") != 0 && strcmp(strrchr(srcFile, '/'), "/..") != 0)
				xcpDir(srcFile, destFile, x_kind, key);
		}
		else if (ret == RET_NO)
			xcpFile(srcFile, destFile, x_kind, key);
	}
	closedir(dir);

	return RET_YES;
}
#endif



// linux windows 完全分为两部分实现；

#if defined(XCP_WIN)

// get dir or filename(with extname) without '/'(linux) '\'(windows)
void getName(const TCHAR *path, TCHAR *name)
{
	TCHAR *tmp;
	TCHAR buf[PATH_MAX] = {};
	size_t len = 0;

	if (path == NULL || name == NULL) return;

	_tcscpy(buf, path);
	len = _tcslen(path);
	if (buf[len - 1] == PATH_DIV)
		buf[len - 1] = '\0';
	tmp = _tcsrchr(buf, PATH_DIV);
	if (tmp == NULL)
		_tcscpy(name, buf);
	else
		_tcscpy(name, tmp + 1);
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


int xcpFile(const TCHAR *srcPath, const TCHAR *destPath, int x_kind, const unsigned char *key)
{
	WIN32_FIND_DATA fd, fd2;
	size_t len;
	time_t ct_src, ct_new;
	TCHAR fileName[NAME_MAX] = _T(""), extName[NAME_MAX] = _T(""), newFile[PATH_MAX] = _T(""), *tmp = NULL;
	char md5[33] = "", path[PATH_MAX] = "", path2[PATH_MAX] = "";
	unsigned char digest[16];

	if (getFD(srcPath, &fd) == RET_ERROR)
		return RET_ERROR;

	// srcpath must be a regular file
	if (isReg(srcPath, &fd) != RET_YES)
		return RET_ERROR;
	
	getExtName(srcPath, extName);
	getName(srcPath, fileName);

	// generate the srcfile's md5sum
	if (x_kind & X_MD5SUM)
	{
		w2c(srcPath, path);
		MD5_File(path, digest, 0, NULL);
		MD5_Str(digest, md5);
		printf("%s    %s\n", md5, path);
		return RET_YES;
	}

	// check the encrypted file
	if (x_kind & X_CHECK)
	{
		// skip the non-encrypt files
		if (_tcscmp(extName, _T(EXT_NAME)) != 0)
			return RET_SKIP;

		w2c(srcPath, path);
		if (X_check(path, key) == RET_YES)
			printf("Check OK.    %s\n", path);
		else
			printf("Check Fail.  %s\n", path);
		return RET_YES;
	}


	len = _tcslen(destPath);
	_tcscpy(newFile, destPath);
	if (PathIsDirectory(destPath))
	{
		if (newFile[len - 1] != _T(PATH_DIV))
			newFile[len] = _T(PATH_DIV);
		_tcscat(newFile, fileName);
	}

	// the encrypted file should have a EXT_NAME
	if (x_kind & X_ENCRYPT)
	{
		tmp = _tcsrchr(newFile, _T('.'));
		if (tmp == NULL || _tcscmp(tmp, _T(EXT_NAME)) != 0)
			_tcscat(newFile, _T(EXT_NAME));
	}
	else if (x_kind & X_DECRYPT)
	{
		len = _tcslen(_T(EXT_NAME));
		if (_tcslen(newFile) <= len)
			return RET_SKIP;
		newFile[_tcslen(newFile) - len] = _T('\0');
	}

	// prevent copy self
	if (_tcscmp(srcPath, newFile) == 0)
	{
		w2c(srcPath, path);
		fprintf(stderr, "Warning : Try to copy self '%s'\n", path);
		return RET_SKIP;
	}

	// update copy
	if (x_kind & X_UPDATE)
	{
		ct_src = getUpdateTime(srcPath, &fd);
		if (getFD(newFile, &fd2) != RET_ERROR)
		{
			ct_new = getUpdateTime(newFile, &fd2);
			if (ct_new >= ct_src)
			{
				printf("Skip  '%s'\n", srcPath);
				return RET_SKIP;
			}
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		w2c
		printf("Encrypt '%s' to '%s'\n", srcPath, newFile);
		X_encrypt(srcPath, newFile, key);
	}
	else if (x_kind & X_DECRYPT)
	{
		// skip the non-encrypt files
		if (strcmp(extName, EXT_NAME) != 0)
			return RET_SKIP;

		printf("Decrypt '%s' to '%s'\n", srcPath, newFile);
		X_decrypt(srcPath, newFile, key);
	}
	else
	{
		printf("Copy '%s' to '%s'\n", srcPath, newFile);
		X_copy(srcPath, newFile);
	}

#if defined(XCP_LINUX)
	// update the file protected mode
	chmod(newFile, getMode(srcPath));
#elif defined(XCP_WIN)

#endif

	return RET_YES;
}



#else
// get dir or filename(with extname) without '/'(linux) '\'(windows)
void getName(const char *path, char *name)
{
	char *tmp;
	char buf[PATH_MAX];
	size_t len = 0;

	if (path == NULL || name == NULL) return;

	memset(buf, 0, PATH_MAX);
	strcpy(buf, path);
	len = strlen(path);
	if (buf[len - 1] == PATH_DIV)
		buf[len - 1] = '\0';
	tmp = strrchr(buf, PATH_DIV);
	if (tmp == NULL)
		strcpy(name, buf);
	else
		strcpy(name, tmp + 1);
}


// get extName with '.', get '.' if no extName
void getExtName(const char *path, char *extName)
{
	const char *tmp;

	if (path == NULL || extName == NULL) return;

	tmp = strrchr(path, '.');
	if (tmp == NULL)
		strcpy(extName, ".");
	else
		strcpy(extName, tmp);
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