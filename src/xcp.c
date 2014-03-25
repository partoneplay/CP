#include "xcp.h"


#if defined(XCP_WIN)


#else
// in linux system

int xcpFile(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key)
{
	size_t len;
	time_t ct_src, ct_new;
	char fileName[NAME_MAX] = "", extName[NAME_MAX] = "", newFile[PATH_MAX] = "";
	char *tmp, md5[33];
	unsigned char digest[16];

	// srcpath must be a regular file
	if (isReg(srcPath) != RET_YES)
	{
		printf("Skip Unregular File '%s'\n", srcPath);
		return RET_SKIP;
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
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		// the encrypted file should have a EXT_NAME
		tmp = strrchr(newFile, '.');
		if (tmp == NULL || strcmp(tmp, EXT_NAME) != 0)
			strcat(newFile, EXT_NAME);
	}
	else if (x_kind & X_DECRYPT)
	{
		// the decrypted file should remove the EXT_NAME
		len = strlen(EXT_NAME);
		if (strlen(newFile) <= len)
		{
			fprintf(stderr, "Unexcepted Error '%s' too short\n", newFile);
			return RET_ERROR;
		}
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

	chmod(newFile, getMode(srcPath));
	return RET_YES;
}


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
				if (strcmp(strrchr(srcFile, '/'), "/.") != 0 && strcmp(strrchr(srcFile, '/'), "/..") != 0)
					xcpDir(srcFile, NULL, x_kind, key);
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

	
    char DirSpec[MAX_PATH + 1];// Ö¸¶¨Â·¾¶
    DWORD dwError;

    memcpy (DirSpec, srcPath, strlen(srcPath) + 1);
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
            printf ("¡¡¡¡%s ", FindFileData.cFileName);   //ÕÒµ½ÎÄ¼þ
        }
        else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
            && strcmp(FindFileData.cFileName, ".") != 0
            && strcmp(FindFileData.cFileName, "..") != 0)
        {   //ÕÒµ½Ä¿Â¼
            char Dir[MAX_PATH + 1];
            strcpy(Dir, srcPath);
            strncat(Dir, "/", 2);
            strcat(Dir, FindFileData.cFileName);

            FindFile(Dir);
        }

        while (FindNextFile(hFind, &FindFileData) != 0)
        {
            if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {   //ÕÒµ½ÎÄ¼þ
                printf ("¡¡¡¡%s ", FindFileData.cFileName);
            }
            else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
                && strcmp(FindFileData.cFileName, ".") != 0
                && strcmp(FindFileData.cFileName, "..") != 0)
            { //ÕÒµ½Ä¿Â¼
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

#endif



// linux windows ÍêÈ«·ÖÎªÁ½²¿·ÖÊµÏÖ£»

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



#endif