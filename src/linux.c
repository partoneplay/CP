#include "linux.h"

/**
struct stat 
{
	dev_t     st_dev;     	// ID of device containing file
	ino_t     st_ino;     	// inode number
	mode_t    st_mode;    	// protection
	nlink_t   st_nlink;   	// number of hard links
	uid_t     st_uid;     	// user ID of owner
	gid_t     st_gid;     	// group ID of owner
	dev_t     st_rdev;    	// device ID (if special file)
	off_t     st_size;    	// total size, in bytes
	blksize_t st_blksize; 	// blocksize for file system I/O
	blkcnt_t  st_blocks;  	// number of 512B blocks allocated
	time_t    st_atime;   	// time of last access
	time_t    st_mtime;   	// time of last modification
	time_t    st_ctime;   	// time of last status change
};

struct dirent {
    ino_t          d_ino;       	inode number
    off_t          d_off;       	offset to the next dirent
    unsigned short d_reclen;    	length of this record
    unsigned char  d_type;      	type of file; not supported by all file system types
    char           d_name[256]; 	filename
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

time_t getCTime(const char *path)
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


int copyFile(const char *srcFile, const char *destPath, int x_kind)
{
	FILE *fin, *fout;
	int index = 0;
	size_t readSize, len;
	char buf[BUF_SIZE] = "", filename[NAME_MAX] = "", newFile[PATH_MAX] = "";
	char *tmp;

	if (strchr(srcFile, '/') != NULL)
	{
		tmp = strrchr(srcFile, '/') + 1;
		memcpy(filename, tmp, strlen(tmp));
	}
	if (x_kind == X_ENCRYPT)
		strcat(filename, EXT_NAME);
	else if (x_kind == X_DECRYPR)
	{
		strcpy(buf, EXT_NAME);
		len = strlen(buf);
		for (index = strlen(filename) - 1; index )
	}
		;// 去除文件后缀名



	len = strlen(destPath);
	memcpy(newFile, destPath, len);
	if (isDir(destPath) == RET_YES)
	{
		if (access(destPath, F_OK) == -1)
			return RET_ERROR;
		else
		{
			// todo;
		}
	}

	// srcFile can not be read or destFile can not be write
	if (access(srcFile, R_OK) == -1 || access(newFile, W_OK) == -1)
		return RET_ERROR;

	fin = fopen(srcPath, "r");
	fout = fopen(newFile, "w");
	if (fin == NULL || fout == NULL)
	{
		if (fin != NULL)
			fclose(fin);
		return RET_ERROR;
	}

	while ((readSize = fread(buf, 1, BUF_SIZE, fin)) > 0)
	{
		fwrite(buf, 1, readSize, fout);
		bzero(buf, BUF_SIZE);
	}
	fclose(fin);
	fclose(fout);

	// update the file protected mode
	chmod(newFile, getMode(srcPath));

	return RET_YES;
}


int copyDir(const char *srcPath, const char *destDir)
{
	int ret;
	DIR *dir;
	struct dirent *pdt;
	char srcFile[PATH_MAX] = "", destFile[PATH_MAX] = "";

	size_t srcDirLen, destDirLen, len;
	char srcPath[PATH_LEN] = "", destPath[PATH_LEN] = "";
	char srcFile[PATH_LEN] = "", destFile[PATH_LEN] = "";

	
	ret = isDir(srcPath);
	if (ret == RET_NO)
	{

		return copyFile(srcPath, destDir);
	}
	else if (ret == RET_ERROR)
		return RET_ERROR;

	dir = opendir(srcPath);
	if (createDir(destDir, getMode(srcDir)) == RET_ERROR)
		return RET_ERROR;	

	bzero(srcPath, PATH_LEN);
	bzero(destPath, PATH_LEN);
	strcpy(srcPath, srcDir);
	strcpy(destPath, destDir);
	if (srcPath[srcDirLen - 1] != '/')
		srcPath[srcDirLen] = '/';
	if (destPath[destDirLen - 1] != '/')
		destPath[destDirLen] = '/';
	while ((pdt = readdir(dir)) != NULL)
	{
		len = strlen(pdt->d_name);
		if (srcDirLen + len >= PATH_LEN - 1 || destDirLen + len >= PATH_LEN - 5)
		{
			fprintf(stderr, "Too long : '%s%s' to '%s%s%s'\n", srcPath, pdt->d_name, destDir, pdt->d_name, EXT);
			continue;
		}
	
		bzero(srcFile, PATH_LEN);
		bzero(destFile, PATH_LEN);

		strcpy(srcFile, srcPath);
		strcat(srcFile, pdt->d_name);
		strcpy(destFile, destPath);
		strcat(destFile, pdt->d_name);
		
		ret = isDir(srcFile);
		if (ret > 0)
		{
			if (strcmp(strrchr(srcFile, '/'), "/.") != 0 && strcmp(strrchr(srcFile, '/'), "/..") != 0)
				copyDir(srcFile, destFile);
		}
		else if (ret == 0)
		{
			strcat(destFile, EXT);
			copyFile(srcFile, destFile);
		}
		else
			return -1;
	}
	closedir(dir);

	return 0;
}