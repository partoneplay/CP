#include "xcp.h"


#if defined(XCP_WIN)

int xcpFile(const TCHAR *srcPath, const TCHAR *destPath, int x_kind, const unsigned char *key)
{
	WIN32_FIND_DATA fd, fd2;
	size_t len;
	time_t ct_src, ct_new;
	TCHAR fileName[NAME_MAX] = _T(""), extName[NAME_MAX] = _T(""), newFile[PATH_MAX] = _T(""), *tmp = NULL;
	char md5[33] = "", path[PATH_MAX] = "", path2[PATH_MAX] = "";
	unsigned char digest[16];

	if (getFD(srcPath, &fd) == RET_ERROR)
	{
		fprintf(stderr, "Fail to get info from '%s' : \n", srcPath, strerror(errno));
		return RET_ERROR;
	}

	// srcpath must be a regular file
	if (isReg(&fd) != RET_YES)
	{
		printf("Skip Unregular File '%s'\n", srcPath);
		return RET_SKIP;
	}
	getExtName(srcPath, extName);
	getName(srcPath, fileName);

	// generate the srcfile's md5sum
	if (x_kind & X_MD5SUM)
	{
		t2c(srcPath, path);
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

		t2c(srcPath, path);
		if (X_check(path, key) == RET_YES)
			printf("Check OK.    %s\n", path);
		else
			printf("Check Fail.  %s\n", path);
		return RET_YES;
	}

	// deal with destPath
	len = _tcslen(destPath);
	_tcscpy(newFile, destPath);
	if (PathIsDirectory(destPath))
	{
		newFile[len] = newFile[len - 1] == _T(PATH_DIV) ? _T('\0') : _T(PATH_DIV);
		_tcscat(newFile, fileName);
		if (x_kind & X_DECRYPT)
		{
			// the decrypted file should remove the EXT_NAME
			len = _tcslen(_T(EXT_NAME));
			{
				fprintf(stderr, "Unexcepted Error '%s' too short\n", newFile);
				return RET_ERROR;
			}
			newFile[_tcslen(newFile) - len] = _T('\0');
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		// the encrypted file should have a EXT_NAME
		tmp = _tcsrchr(newFile, _T('.'));
		if (tmp == NULL || _tcscmp(tmp, _T(EXT_NAME)) != 0)
			_tcscat(newFile, _T(EXT_NAME));
	}

	// prevent copy self
	if (_tcscmp(srcPath, newFile) == 0)
	{
		t2c(srcPath, path);
		fprintf(stderr, "Warning : Try to copy self '%s'\n", path);
		return RET_SKIP;
	}

	// update copy
	if (x_kind & X_UPDATE)
	{
		ct_src = getUpdateTime(&fd);
		if (getFD(newFile, &fd2) != RET_ERROR)
		{
			ct_new = getUpdateTime(&fd2);
			if (ct_new >= ct_src)
			{
				t2c(srcPath, path);
				printf("Skip  '%s'\n", path);
				return RET_SKIP;
			}
		}
	}

	t2c(srcPath, path);
	t2c(newFile, path2);
	if (x_kind & X_ENCRYPT)
	{
		printf("Encrypt '%s' to '%s'\n", path, path2);
		X_encrypt(path, path2, key);
	}
	else if (x_kind & X_DECRYPT)
	{
		// skip the non-encrypt files
		if (_tcscmp(extName, _T(EXT_NAME)) != 0)
			return RET_SKIP;

		printf("Decrypt '%s' to '%s'\n", path, path2);
		X_decrypt(path, path2, key);
	}
	else
	{
		printf("Copy '%s' to '%s'\n", path, path2);
		X_copy(path, path2);
	}

	return RET_YES;
}


int xcp(const TCHAR *srcPath, const TCHAR *destPath, int x_kind, const unsigned char *key)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	int ret = 0;
	size_t len = 0;
	char path[PATH_MAX] = "";
	TCHAR dirName[NAME_MAX] = _T(""), dirPattern[PATH_MAX] = _T("");
	TCHAR srcFile[PATH_MAX] = _T(""), destFile[PATH_MAX] = _T("");
	TCHAR srcBase[PATH_MAX] = _T(""), destBase[PATH_MAX] = _T("");

	if (getFD(srcPath, &fd) == RET_ERROR)
	{
		fprintf(stderr, "Fail to get info from '%s'\n", srcPath);
		return RET_ERROR;
	}
	
	ret = isDir(&fd);
	if (ret == RET_NO)
		return xcpFile(srcPath, destPath, x_kind, key);	
	else if (ret == RET_ERROR)
		return RET_ERROR;
	getName(srcPath, dirName);

	len = _tcslen(srcPath);
	_tcscpy(srcBase, srcPath);
	srcBase[len] = srcPath[len - 1] == _T(PATH_DIV) ? _T('\0') : _T(PATH_DIV);
	
	_tcscpy(dirPattern, srcBase);
	_tcscat(dirPattern, _T("*"));
	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	{
		hFind = FindFirstFile(dirPattern, &fd);
		while (hFind != INVALID_HANDLE_VALUE)
		{
			_tcscpy(srcFile, srcBase);
			_tcscat(srcFile, fd.cFileName);

			ret = isDir(&fd);
			if (ret == RET_YES)
			{
				if (_tcscmp(fd.cFileName, _T(".")) != 0 && _tcscmp(fd.cFileName, _T("..")) != 0)
					xcp(srcFile, NULL, x_kind, key);
			}
			else if (ret == RET_NO)
				xcpFile(srcFile, NULL, x_kind, key);

			if (!FindNextFile(hFind, &fd))
			{
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
			}
		}
		return RET_YES;
	}

	// deal with destPath
	len = _tcslen(destPath);
	_tcscpy(destBase, destPath);
	destBase[len] = destBase[len - 1] == _T(PATH_DIV) ? _T('\0') :_T(PATH_DIV);
	if (PathIsDirectory(destBase))
	{
		_tcscat(destBase, dirName);
		_tcscat(destBase, _T("\\"));
	}
	if (createDir(destBase) == RET_ERROR)
	{
		t2c(destBase, path);
		fprintf(stderr, "Fail to Create Dir '%s' : %s\n", path, strerror(errno));
		return RET_ERROR;
	}

	hFind = FindFirstFile(dirPattern, &fd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		_tcscpy(srcFile, srcBase);
		_tcscat(srcFile, fd.cFileName);

		_tcscpy(destFile, destBase);
		_tcscat(destFile, fd.cFileName);

		ret = isDir(&fd);
		if (ret == RET_YES)
		{
			if (_tcscmp(fd.cFileName, _T(".")) != 0 && _tcscmp(fd.cFileName, _T("..")) != 0)
				xcp(srcFile, destFile, x_kind, key);
		}
		else if (ret == RET_NO)
			xcpFile(srcFile, destFile, x_kind, key);

		if (!FindNextFile(hFind, &fd))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
	}
	return RET_YES;
}


#else
// in linux system

int xcpFile(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key)
{
	int ret = 0;
	size_t len;
	time_t ct_src, ct_new;
	char fileName[NAME_MAX] = "", extName[NAME_MAX] = "", newFile[PATH_MAX] = "";
	char *tmp, md5[33];
	unsigned char digest[16];

	// srcpath must be a regular file
	ret = isReg(srcPath);
	if (ret == RET_NO)
	{
		printf("Skip Unregular File '%s'\n", srcPath);
		return RET_SKIP;
	}
	else if (ret == RET_ERROR)
	{
		fprintf(stderr, "Fail to get info form '%s' : %s\n", srcPath, strerror(errno));
		return RET_ERROR;
	} 
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

	// deal with destPath
	len = strlen(destPath);
	memcpy(newFile, destPath, len);
	if (isDir(destPath) == RET_YES)
	{
		// the dir must exists
		if (access(destPath, F_OK) == -1)
		{
			fprintf(stderr, "'%s' not Exists\n", destPath);
			return RET_ERROR;
		}
		else
		{
			newFile[len] = newFile[len - 1] == PATH_DIV ? '\0' : PATH_DIV;
			strcat(newFile, fileName);
			if (x_kind & X_DECRYPT)
			{
				// the decrypted file should remove the EXT_NAME
				len = strlen(EXT_NAME);
				if (strlen(newFile) <= len)
				{
					fprintf(stderr, "Error '%s' too short\n", newFile);
					return RET_ERROR;
				}
				memset(&newFile[strlen(newFile) - len], 0, len);
			}
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		// the encrypted file should have a EXT_NAME
		tmp = strrchr(newFile, '.');
		if (tmp == NULL || strcmp(tmp, EXT_NAME) != 0)
			strcat(newFile, EXT_NAME);
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

	chmod(newFile, getMode(srcPath));
	return RET_YES;
}


int xcp(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key)
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

	// for check and md5sum
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
				if (strcmp(pdt->d_name, ".") != 0 && strcmp(pdt->d_name, "..") != 0)
					xcp(srcFile, NULL, x_kind, key);
			}
			else if (ret == RET_NO)
				xcpFile(srcFile, NULL, x_kind, key);
		}
		closedir(dir);
		return RET_YES;
	}
		
	// deal with destPath
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
		strcpy(srcFile, srcBase);
		strcat(srcFile, pdt->d_name);

		memset(destFile, 0, PATH_MAX);
		strcpy(destFile, destBase);
		strcat(destFile, pdt->d_name);

		ret = isDir(srcFile);
		if (ret == RET_YES)
		{
			if (strcmp(pdt->d_name, ".") != 0 && strcmp(pdt->d_name, "..") != 0)
				xcp(srcFile, destFile, x_kind, key);
		}
		else if (ret == RET_NO)
			xcpFile(srcFile, destFile, x_kind, key);
	}
	closedir(dir);
	return RET_YES;
}

#endif
