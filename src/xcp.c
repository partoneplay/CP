#include "xcp.h"

// get dir or filename(with extname) without '/'(linux) '\'(windows)
void getName(const char *path, char *name)
{
	char *tmp;
	char div = '\0';
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
	char *tmp;

	if (path == NULL || extName == NULL) return;

	tmp = strrchr(path, '.');
	if (tmp == NULL)
		strcpy(extName, ".");
	else
		strcpy(extName, tmp);
}


#if defined(XCP_WIN)
#define F_OK 0
#define access _access

/*
typedef struct _WIN32_FIND_DATA
{
	DWORD dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD dwReserved0;
	DWORD dwReserved1;
	TCHAR cFileName[MAX_PATH];
	TCHAR cAlternateFileName[14];
} WIN32_FIND_DATA;
*/

int isDir(const char* path)
{
	if (PathIsDirectory((LPCTSTR)path))
		return RET_YES;
	else
		return RET_NO;
}

time_t getUpdateTime(const char *path)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(path, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return RET_ERROR;
	else
	{
		FindClose(hFind);
//		return FindFileData.ftLastWriteTime;
	}
	return 1;
}

#else
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
	if (mkdir(path, mode) == -1 && errno != EEXIST)
		return RET_ERROR;
	return RET_YES;
}
#endif


int xcpFile(const char *srcPath, const char *destPath, int x_kind, const char *key)
{
	size_t len;
	time_t ct_src, ct_new;
	char fileName[NAME_MAX] = "", extName[NAME_MAX] = "", newFile[PATH_MAX] = "";
	char *tmp, digest[16], md5[33];

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
int xcpDir(const char *srcPath, const char *destDir, int x_kind, const char *key)
{
	return xcpFile(srcPath, destDir, x_kind, key);
}


#else
int xcpDir(const char *srcPath, const char *destPath, int x_kind, const char *key)
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