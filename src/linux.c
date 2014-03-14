#include "linux.h"

#define debug printf

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


int isDir(const char* path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return -1;
	return S_ISDIR(st.st_mode & S_IFMT);
}

off_t getSize(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		fprintf(stderr, "Size '%s' : %s\n", path, strerror(errno));
		return 0;
	return st.st_size;
}

mode_t getMode(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return 0;
	return st.st_mode;
}

time_t getCTime(const char *path)
{
	struct stat st;
	if (stat(path, &st) == -1)
		return 0;
	return st.st_ctime;
}


int createDir(const char* path, mode_t mode)
{
	if (mkdir(path, mode) == -1 && errno != EEXIST)
	{
		fprintf(stderr, "Create dir '%s' Fail : %s\n", path, strerror(errno));
		return -1;
	}
	debug("Create Dir '%s' Complete\n", path);
	return 0;
}


int copyFile(const char* srcPath, const char* destPath)
{
	FILE *fin, *fout;
	size_t readSize, len;
	char filename[MAX_LEN] = "";
	char newPath[MAX_LEN] = "";
	char buf[BUF_SIZE];

	debug("Copy file '%s' to '%s'\n", srcPath, destPath);

	// deal with srcPath
	if (access(srcPath, R_OK) == -1)
	{
		fprintf(stderr, "Read '%s' : %s\n", srcPath, strerror(errno));
		return -1;
	}
	if (strchr(srcPath, '/'))
		strcpy(filename, strrchr(srcPath, '/') + 1);
	else
		strcpy(filename, srcPath);

	// deal with destPath
	if (isDir(destPath) > 0 && access(destPath, F_OK) == -1)
	{
		fprintf(stderr, "Dir '%s' : %s\n", destPath, strerror(errno));
		return -1;	
	}
	strcpy(newPath, destPath);
	if (isDir(newPath) > 0)
	{
		len = strlen(newPath);
		if (len + strlen(filename) >= MAX_LEN - 1)
		{
			fprintf(stderr, "Too long : '%s'\n", destPath);
			return -1;
		}
		if (newPath[len - 1] != '/')
			newPath[len] = '/';
		strcat(newPath, filename);
	}

	debug("Open '%s' to read\nOpen '%s' to write\n", srcPath, newPath);
	
	fin = fopen(srcPath, "r");
	fout = fopen(newPath, "w");
	if (fin == NULL || fout == NULL)
	{
		if (fin != NULL)
			fclose(fin);
		debug("Fin : %p  Fout : %p\n", fin, fout);
		return -1;
	}

	while ((readSize = fread(buf, 1, BUF_SIZE, fin)) > 0)
	{
		fwrite(buf, 1, readSize, fout);
		bzero(buf, BUF_SIZE);
	}
	fclose(fin);
	fclose(fout);

	// update the file mode
	chmod(newPath, getMode(srcPath));

	debug("Copy file '%s' to '%s' Complete\n", srcPath, destPath);

	return 0;
}


int copyDir(const char* srcDir, const char* destDir)
{
	DIR *dir;
	struct dirent *pdt;
	int ret;
	size_t srcDirLen, destDirLen, len;
	char srcPath[MAX_LEN] = "", destPath[MAX_LEN] = "";
	char srcFile[MAX_LEN] = "", destFile[MAX_LEN] = "";

	srcDirLen = strlen(srcDir);
	destDirLen = strlen(destDir);
	if (srcDirLen >= MAX_LEN - 1 || destDirLen >= MAX_LEN - 1)
	{
		fprintf(stderr, "Too long : '%s' to '%s'\n", srcDir, destDir);
		return -1;
	}

	debug("Copy dir '%s' to '%s'\n", srcDir, destDir);

	ret = isDir(srcDir);
	if (ret == 0)
		return copyFile(srcDir, destDir);
	else if (ret == -1)
	{
		fprintf(stderr, "'%s' : %s\n", srcDir, strerror(errno));
		return -1;
	}

	dir = opendir(srcDir);
	if (createDir(destDir, getMode(srcDir)) == -1)
		return -1;	

	bzero(srcPath, MAX_LEN);
	bzero(destPath, MAX_LEN);
	strcpy(srcPath, srcDir);
	strcpy(destPath, destDir);
	if (srcPath[srcDirLen - 1] != '/')
		srcPath[srcDirLen] = '/';
	if (destPath[destDirLen - 1] != '/')
		destPath[destDirLen] = '/';
	while ((pdt = readdir(dir)) != NULL)
	{
		len = strlen(pdt->d_name);
		if (srcDirLen + len >= MAX_LEN - 1 || destDirLen + len >= MAX_LEN - 5)
		{
			fprintf(stderr, "Too long : '%s%s' to '%s%s%s'\n", srcPath, pdt->d_name, destDir, pdt->d_name, EXT);
			continue;
		}
	
		bzero(srcFile, MAX_LEN);
		bzero(destFile, MAX_LEN);

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