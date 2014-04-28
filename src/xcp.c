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
		mod = mod == 0 ? 16 : mod;	// 16x
		memset(readBuf, 0, BUF_SIZE);
	}
	MD5_Update(&context, (unsigned char*)&mod, 1);
	MD5_Final(&context, buf);
	fseek(fout, 0, SEEK_SET);
	fwrite(buf, 16, 1, fout);	// md5sum
	fwrite(&mod, 1, 1, fout);	// mod
	fclose(fin);
	fclose(fout);

	return RET_YES;
}

int X_decrypt(const char *srcFile, const char *newFile, const unsigned char *userKey)
{
	AES_KEY key;
	FILE *fin, *fout;
	MD5Context context;
	unsigned char digest[16 + 1] = "";
	unsigned char buf[16 + 1] = "";
	unsigned char readBuf[BUF_SIZE] = "";
	size_t i, readSize = 0, lastSize = 0;
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

	AES_set_decrypt_key(userKey, 128, &key);
	fread(digest, 16, 1, fin);
	fread(&mod, 1, 1, fin);
	MD5_Init(&context);
	MD5_Update(&context, userKey, strlen((char*)userKey));
	readSize = fread(readBuf, 1, BUF_SIZE, fin);
	while (readSize > 0)
	{
		for (i = 0; i < readSize - 16; i += 16)
		{
			MD5_Update(&context, &readBuf[i], 16);
			AES_decrypt(&readBuf[i], buf, &key);
			fwrite(buf, 16, 1, fout);
		}
		MD5_Update(&context, &readBuf[i], 16);
		AES_decrypt(&readBuf[i], buf, &key);

		lastSize = readSize;
		memset(readBuf, 0, BUF_SIZE);
		readSize = fread(readBuf, 1, BUF_SIZE, fin);
		
		if (lastSize < BUF_SIZE || readSize == 0)
			fwrite(buf, mod, 1, fout);	// relate to encrypt
		else
			fwrite(buf, 16, 1, fout);
	}
	MD5_Update(&context, (unsigned char*)&mod, 1);
	MD5_Final(&context, buf);
	fclose(fin);
	fclose(fout);

	if (memcmp(digest,buf, 16) == 0)
		return RET_YES;
	else
	{
		deleteFile(newFile);
		fprintf(stderr, "Check Fail : %s\n", srcFile);
		return RET_NO;
	}
}

int X_check(const char *filename, const unsigned char *userKey)
{
	FILE *file;
	char mod = 0;
	unsigned char buf[16] = "", digest[16] = "";

	file = fopen(filename, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Read File Error, Fail to check '%s'\n", filename);
		return RET_ERROR;
	}
	fread(buf, 16, 1, file);
	fread(&mod, 1, 1, file);
	fclose(file);

	MD5_File(filename, digest, 17, userKey, strlen(userKey), (unsigned char*)&mod, 1);
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
		if (x_kind & X_VIEW)
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
		MD5_File(srcPath, digest, 0, NULL, 0, NULL, 0);
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
			if (x_kind & X_VIEW)
				printf("Skip  '%s'\n", srcPath);
			return RET_SKIP;
		}
	}

	if (x_kind & X_ENCRYPT)
	{
		if (x_kind & X_VIEW)
			printf("Encrypt '%s' to '%s'\n", srcPath, newFile);
		ret = X_encrypt(srcPath, newFile, key);
	}
	else if (x_kind & X_DECRYPT)
	{
		// skip the non-encrypt files
		if (strcmp(extName, EXT_NAME) != 0)
			return RET_SKIP;

		if (x_kind & X_VIEW)
			printf("Decrypt '%s' to '%s'\n", srcPath, newFile);
		ret = X_decrypt(srcPath, newFile, key);
	}
	else
	{
		if (x_kind & X_VIEW)
			printf("Copy '%s' to '%s'\n", srcPath, newFile);
		ret = X_copy(srcPath, newFile);
	}
#if defined(XCP_WIN)
	;
#else
	chmod(newFile, getMode(srcPath));
#endif
	return ret;
}


int xcp(const char *srcPath, const char *destPath, int x_kind, const unsigned char *pkey)
{
	X_DIR xdir;
	
	int ret = 0;
	size_t len = 0;
	char dirName[NAME_MAX] = "";
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
		ret = X_findfirst(srcBase, &xdir);
		if (ret != RET_YES)
			return RET_END;

		while (ret != RET_END)
		{
			strcpy(srcFile, srcBase);
			strcat(srcFile, xdir.name);

			ret = isDir(srcFile);
			if (ret == RET_YES)
			{
				if (strcmp(xdir.name, ".") != 0 && strcmp(xdir.name, "..") != 0)
					xcp(srcFile, NULL, x_kind, pkey);
			}
			else if (ret == RET_NO)
				xcpFile(srcFile, NULL, x_kind, pkey);

			ret = X_findnext(&xdir);
		}
		X_findclose(&xdir);
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

	ret = X_findfirst(srcBase, &xdir);
	if (ret != RET_YES)
		return RET_END;

	while (ret != RET_END)
	{
		strcpy(srcFile, srcBase);
		strcat(srcFile, xdir.name);

		strcpy(destFile, destBase);
		strcat(destFile, xdir.name);

		ret = isDir(srcFile);
		if (ret == RET_YES)
		{
			if (strcmp(xdir.name, ".") != 0 && strcmp(xdir.name, "..") != 0)
				xcp(srcFile, destFile, x_kind, pkey);
		}
		else if (ret == RET_NO)
			xcpFile(srcFile, destBase, x_kind, pkey);

		ret = X_findnext(&xdir);
	}
	X_findclose(&xdir);
	return RET_YES;
}