#include "xcp.h"
#include "aes.h"
#include "md5.h"
#include "func.h"

int X_encrypt(const char *srcFile, const char *newFile, const unsigned char *userKey)
{
	AES_KEY key;
	FILE *fin, *fout;
	MD5Context context;
	unsigned char buf[16 + 1] = "";
	unsigned char readBuf[BUF_SIZE] = "";
	size_t i, readSize = 0;
	unsigned char mod = 0;

	fin = fopen(srcFile, "rb");
	fout = fopen(newFile, "wb");
	if (fin == NULL || fout == NULL)
	{
		if (fin != NULL)
			fclose(fin);
		fprintf(stderr, "Fail to copy '%s'\n", srcFile);
		return RET_ERROR;
	}

	AES_set_encrypt_key(userKey, 128, &key);
	memset(buf, 0, 17);
	fwrite(buf, 17, 1, fout);	// file head
	MD5_Init(&context);
	MD5_Update(&context, userKey, strlen((char*)userKey));
	while ((readSize = fread(readBuf, 1, BUF_SIZE, fin)) > 0)
	{
		for (i = 0; i < readSize; i += 16)
		{
			AES_encrypt(&readBuf[i], buf, &key);
			MD5_Update(&context, buf, 16);
			fwrite(buf, 16, 1, fout);
		}
		mod = readSize % 16;
		memset(readBuf, 0, BUF_SIZE);
	}
	MD5_Final(&context, buf);
	fseek(fout, 0, SEEK_SET);
	fwrite(buf, 16, 1, fout);	// md5sum
	fwrite(&mod, 1, 1, fout);	// filesize % 16
	fclose(fin);
	fclose(fout);

	return RET_YES;
}

int X_decrypt(const char *srcFile, const char *newFile, const unsigned char *userKey)
{
	AES_KEY key;
	FILE *fin, *fout;
	unsigned char buf[16 + 1] = "";
	unsigned char readBuf[BUF_SIZE] = "";
	size_t i, readSize = 0;
	unsigned char mod = 0;

	if (X_check(srcFile, userKey) == RET_NO)
	{
		fprintf(stderr, "'%s' Checksum Fail\n", srcFile);
		return RET_NO;
	}

	fin = fopen(srcFile, "rb");
	fout = fopen(newFile, "wb");
	if (fin == NULL || fout == NULL)
	{
		if (fin != NULL)
			fclose(fin);
		fprintf(stderr, "Fail to copy '%s'\n", srcFile);
		return RET_ERROR;
	}

	AES_set_decrypt_key(userKey, 128, &key);
	fread(buf, 16, 1, fin);
	fread(&mod, 1, 1, fin);
	while ((readSize = fread(readBuf, 1, BUF_SIZE, fin)) > 0)
	{
		for (i = 0; i < readSize - 16; i += 16)
		{
			AES_decrypt(&readBuf[i], buf, &key);
			fwrite(buf, 16, 1, fout);
		}
		AES_decrypt(&readBuf[i], buf, &key);
		if (readSize < BUF_SIZE)
			fwrite(buf, mod, 1, fout);
		else
			fwrite(buf, 16, 1, fout);
		memset(readBuf, 0, BUF_SIZE);
	}
	fclose(fin);
	fclose(fout);

	return RET_YES;
}

int X_check(const char *filename, const unsigned char *userKey)
{
	FILE *file;
	unsigned char buf[16] = "", digest[16] = "";

	file = fopen(filename, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Read File Error, Fail to check '%s'\n", filename);
		return RET_ERROR;
	}
	fread(buf, 16, 1, file);
	fclose(file);

	MD5_File(filename, digest, 17, userKey);
	if (memcmp(buf, digest, 16) == 0)
		return RET_YES;
	else
		return RET_NO;
}

int X_copy(const char *srcFile, const char *newFile)
{
	FILE *fin, *fout;
	char readBuf[BUF_SIZE] = "";
	size_t readSize = 0;

	fin = fopen(srcFile, "rb");
	fout = fopen(newFile, "wb");
	if (fin == NULL || fout == NULL)
	{
		if (fin != NULL)
			fclose(fin);
		fprintf(stderr, "Fail to copy '%s'\n", srcFile);
		return RET_ERROR;
	}

	while ((readSize = fread(readBuf, 1, BUF_SIZE, fin)) > 0)
	{
		fwrite(readBuf, readSize, 1, fout);
		memset(readBuf, 0, BUF_SIZE);
	}
	fclose(fin);
	fclose(fout);

	return RET_YES;
}


int xcpFile(const char *srcPath, const char *destPath, int x_kind, const unsigned char *key)
{
	int ret = 0;
	size_t len;
	time_t ct_src, ct_new;
	char fileName[NAME_MAX] = "", extName[NAME_MAX] = "", newFile[PATH_MAX] = "";
	char *tmp, md5[33];
	unsigned char digest[16];

	// srcpath must not be a Dir
	ret = isDir(srcPath);
	if (ret == RET_YES)
	{
		printf("Skip Folder '%s'\n", srcPath);
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
		ct_src = getLastUpdateTime(srcPath);
		ct_new = getLastUpdateTime(newFile);
		if (ct_new >= ct_src)
		{
			printf("Skip  '%s'\n", srcPath);
			return RET_SKIP;
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		printf("Encrypt '%s' to '%s'\n", srcPath, newFile);
		return X_encrypt(srcPath, newFile, key);
	}
	else if (x_kind & X_DECRYPT)
	{
		// skip the non-encrypt files
		if (strcmp(extName, EXT_NAME) != 0)
			return RET_SKIP;

		printf("Decrypt '%s' to '%s'\n", srcPath, newFile);
		return X_decrypt(srcPath, newFile, key);
	}
	else
	{
		printf("Copy '%s' to '%s'\n", srcPath, newFile);
		return X_copy(srcPath, newFile);
	}
#if defined(XCP_WIN)
#define something
#else
	chmod(newFile, getMode(srcPath));
#endif
	return RET_YES;
}


int xcp(const char *srcPath, const char *destPath, int x_kind, const unsigned char *pkey)
{
#if defined (XCP_WIN)
	WIN32_FIND_DATA fd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR tmp_t[PATH_MAX] = _T("");
#else
	DIR *dir;
	struct dirent *pdt;
#endif
	
	int ret = 0;
	size_t len = 0;
	char dirName[NAME_MAX] = "", tmp[PATH_MAX] = "";
	char srcFile[PATH_MAX] = "", destFile[PATH_MAX] = "";
	char srcBase[PATH_MAX] = "", destBase[PATH_MAX] = "";

	ret = isDir(srcPath);
	if (ret == RET_NO)
		return xcpFile(srcPath, destPath, x_kind, pkey);	
	else if (ret == RET_ERROR)
		return RET_ERROR;
	getName(srcPath, dirName);

	len = strlen(srcPath);
	strcpy(srcBase, srcPath);
	srcBase[len] = srcPath[len - 1] == PATH_DIV ? '\0' : PATH_DIV;

	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	{
#if defined(XCP_WIN)
		c2t(srcBase, tmp_t);
		_tcscat(tmp_t, _T("*"));
		hFind = FindFirstFile(tmp_t, &fd);
		while (hFind != INVALID_HANDLE_VALUE)
		{
			t2c(fd.cFileName, tmp);
#else
		dir = opendir(srcBase);		
		while ((pdt = readdir(dir)) != NULL)
		{
			strcpy(tmp, pdt->d_name);
#endif
			strcpy(srcFile, srcBase);
			strcat(srcFile, tmp);

			ret = isDir(srcFile);
			if (ret == RET_YES)
			{
				if (strcmp(tmp, ".") != 0 && strcmp(tmp, "..") != 0)
					xcp(srcFile, NULL, x_kind, pkey);
			}
			else if (ret == RET_NO)
				xcpFile(srcFile, NULL, x_kind, pkey);
#if defined(XCP_WIN)
			if (!FindNextFile(hFind, &fd))
			{
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
			}
#endif
		}
#ifndef XCP_WIN
		closedir(dir);
#endif
		return RET_YES;
	}

	// deal with destPath
	len = strlen(destPath);
	strcpy(destBase, destPath);
	destBase[len] = destBase[len - 1] == PATH_DIV ? '\0' : PATH_DIV;
	if (isDir(destBase) == RET_YES)
	{
		strcat(destBase, dirName);
		len = strlen(destBase);
		destBase[len] = destBase[len - 1] == PATH_DIV ? '\0' : PATH_DIV;
	}
#if defined(XCP_WIN)
	if (createDir(destBase) == RET_ERROR)
#else
	if (createDir(destBase, getMode(srcBase)) == RET_ERROR)
#endif
	{
		fprintf(stderr, "Fail to Create Dir '%s' : %s\n", destBase, strerror(errno));
		return RET_ERROR;
	}

#if defined(XCP_WIN)
	c2t(srcBase, tmp_t);
	_tcscat(tmp_t, _T("*"));
	hFind = FindFirstFile(tmp_t, &fd);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		t2c(fd.cFileName, tmp);
#else
	dir = opendir(srcBase);	
	while ((pdt = readdir(dir)) != NULL)
	{
		strcpy(tmp, pdt->d_name);
#endif
		strcpy(srcFile, srcBase);
		strcat(srcFile, tmp);

		strcpy(destFile, destBase);
		strcat(destFile, tmp);

		ret = isDir(srcFile);
		if (ret == RET_YES)
		{
			if (strcmp(tmp, ".") != 0 && strcmp(tmp, "..") != 0)
				xcp(srcFile, destFile, x_kind, pkey);
		}
		else if (ret == RET_NO)
			xcpFile(srcFile, destBase, x_kind, pkey);

#if defined(XCP_WIN)
		if (!FindNextFile(hFind, &fd))
		{
			FindClose(hFind);
			hFind = INVALID_HANDLE_VALUE;
		}
#endif
	}
#ifndef XCP_WIN
	closedir(dir);
#endif
	return RET_YES;
}