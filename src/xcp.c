#include "xcp.h"


int dealExt(char *filename, int x_kind)
{
	size_t len = 0, extlen = 0;
	char buf[NAME_MAX] = "";
	
	len = strlen(filename);
	extlen = strlen(EXT_NAME);

	if (len <= extlen && (x_kind & X_DECRYPT || x_kind & X_CHECK))
		return X_NONE;

	if (x_kind & X_ENCRYPT)
	{
		strcat(filename, EXT_NAME);
		return X_ENCRYPT;
	}
	else if (x_kind & X_DECRYPT)
	{
		// the extname must be EXT_NAME
		strcpy(buf, &filename[len]);
		memcpy(buf, &filename[len - extlen], extlen);
		if (strcmp(buf, EXT_NAME) == 0)
		{
			memset(&filename[len - extlen], 0, extlen);
		}
		else 
			return X_NONE;
	}
	else if (x_kind & X_CHECK)
	{
		if (strcmp(strrchr(filename, '.'), EXT_NAME) == 0)
			return X_CHECK;
		else
			return X_NONE;
	}
	else
		return X_NONE;
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
/**
struct stat 
{
dev_t	 st_dev;	 	// ID of device containing file
ino_t	 st_ino;	 	// inode number
mode_t	st_mode;		// protection
nlink_t   st_nlink;   	// number of hard links
uid_t	 st_uid;	 	// user ID of owner
gid_t	 st_gid;	 	// group ID of owner
dev_t	 st_rdev;		// device ID (if special file)
off_t	 st_size;		// total size, in bytes
blksize_t st_blksize; 	// blocksize for file system I/O
blkcnt_t  st_blocks;  	// number of 512B blocks allocated
time_t	st_atime;   	// time of last access
time_t	st_mtime;   	// time of last modification
time_t	st_ctime;   	// time of last status change
};

struct dirent {
ino_t		  d_ino;	   	inode number
off_t		  d_off;	   	offset to the next dirent
unsigned short d_reclen;		length of this record
unsigned char  d_type;	  	type of file; not supported by all file system types
char		   d_name[256]; 	filename
};
*/


int isDir(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return S_ISDIR(st.st_mode & S_IFMT) > 0 ? RET_YES : RET_NO;
}


off_t getSize(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return st.st_size;
}

mode_t getMode(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return st.st_mode;
}

time_t getUpdateTime(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return RET_ERROR;
	return st.st_ctime;
}


int createDir(const char *path, mode_t mode)
{
	if (mkdir(path, mode) == -1 && errno != EEXIST)
		return RET_ERROR;
	return RET_YES;
}

void getName(const char *path, char *name)
{
	char buf[PATH_MAX];
	char *tmp;
	size_t len = 0;

	if (path == NULL)
		return;

	memset(buf, 0, PATH_MAX);
	strcpy(buf, path);
	len = strlen(path);
	if (buf[len - 1] == '/')
		buf[len - 1] = '\0';
	tmp = strrchr(buf, '/');
	if (tmp == NULL)
		strcpy(name, buf);
	else
		strcpy(name, tmp + 1);
}

#endif


int xcpFile(const char *srcFile, const char *destPath, int x_kind, const char *key)
{
	int ret = 0;
	size_t len;
	time_t ct_src, ct_new;
	char buf[BUF_SIZE] = "", filename[NAME_MAX] = "", newFile[PATH_MAX] = "";
	char *tmp, digest[16], md5[33];

	// md5 function
	if (x_kind & X_MD5SUM)
	{
		MD5_File(srcFile, digest, 0, NULL);
		MD5_Str(digest, md5);
		printf("%s    %s\n", md5, srcFile);
		return RET_YES;
	}

	if (strchr(srcFile, '/') != NULL)
	{
		tmp = strrchr(srcFile, '/') + 1;
		memcpy(filename, tmp, strlen(tmp));
	}
	else
		memcpy(filename, srcFile, strlen(srcFile));
	ret = dealExt(filename, x_kind);

	// check function
	if (x_kind & X_CHECK)
	{
		// skip the non-encrypt files
		if (ret == X_NONE)
			return RET_NO;

		if (X_check(srcFile, key) == RET_YES)
			printf("Check OK.    %s\n", srcFile);
		else 
			printf("Check Fail.  %s\n", srcFile);
		return RET_YES;
	}

	// skip the non-encrypt files
	if (ret == X_NONE && x_kind & X_DECRYPT)
		return RET_NO;

	len = strlen(destPath);
	memcpy(newFile, destPath, len);
	if (isDir(destPath) == RET_YES)
	{
		if (access(destPath, F_OK) == -1)
			return RET_ERROR;
		else
		{
			if (newFile[len - 1] != '/')
				newFile[len] = '/';
			strcat(newFile, filename);
		}
	}
	else if (x_kind & X_ENCRYPT)
	{
		tmp = strrchr(newFile, '.');
		if (tmp == NULL || strcmp(tmp, EXT_NAME) != 0)
			strcat(newFile, EXT_NAME);
	}

	// prevent copy self
	if (strcmp(srcFile, newFile) == 0 && x_kind != X_CHECK && x_kind != X_MD5SUM)
	{
		fprintf(stderr, "Warning : Try to copy self '%s'\n", srcFile);
		return RET_NO;
	}

	// update copy
	if (x_kind & X_UPDATE)
	{
		ct_src = getUpdateTime(srcFile);
		ct_new = getUpdateTime(newFile);
	//	printf("%ld %ld\n", ct_src, ct_new);
		if (ct_new >= ct_src)
		{
			printf("Skip  '%s'\n", srcFile);
			return RET_NO;
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		printf("Encrypt '%s' to '%s' with '%s'\n", srcFile, newFile, key);
		X_encrypt(srcFile, newFile, key);
	}
	else if (x_kind & X_DECRYPT)
	{
		printf("Decrypt '%s' to '%s' with %s'\n", srcFile, newFile, key);
		X_decrypt(srcFile, newFile, key);
	}
	else
	{
		printf("Copy '%s' to '%s'\n", srcFile, newFile);
		X_copy(srcFile, newFile);
	}

#if defined(XCP_LINUX)
	// update the file protected mode
	chmod(newFile, getMode(srcFile));
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
int xcpDir(const char *srcPath, const char *destDir, int x_kind, const char *key)
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
		return xcpFile(srcPath, destDir, x_kind, key);	
	else if (ret == RET_ERROR)
		return RET_ERROR;
	dir = opendir(srcPath);
	getName(srcPath, dirName);

	len = strlen(srcPath);
	strcpy(srcBase, srcPath);
	srcBase[len] = srcPath[len - 1] == '/' ? '\0' : '/';

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

		return RET_YES;
	}
		
	len = strlen(destDir);
	strcpy(destBase, destDir);
	destBase[len] = destDir[len - 1] == '/' ? '\0' : '/';
	if (isDir(destDir) == RET_YES)
	{
		strcat(destBase, dirName);
		strcat(destBase, "/");
	}
	if (createDir(destBase, getMode(srcPath)) == RET_ERROR)
	{
		fprintf(stderr, "Fail to Create Dir '%s' %s\n", destDir, strerror(errno));
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
		{
			dealExt(destFile, x_kind);
			xcpFile(srcFile, destFile, x_kind, key);
		}
	}
	closedir(dir);

	return RET_YES;
}
#endif