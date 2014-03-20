#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linux.h"
#include "aes.h"
#include "md5.h"

#define KEY_LEN 128

/*
void help(const char *opt)
{
	if (opt == NULL || strlen(opt) < 2)
	{

	}
	else if (opt[1] == 'd')
		printf("Do you mean '-d | --decrypt key'\n");
	else if (opt[1] == 'e')
		printf("Do you mean '-e | --encrypt key'\n");
	else if (opt[1] == 'r')
		printf("Do you mean '-r | --recursive'\n");
	else if (opt[1] == 'u')
		printf("Do you mean '-u | --update'\n");
	else if (opt[1] == 'c')
		printf("Do you mean '-c | --check'\n");
}
*/

int main(int argc, char **argv)
{
	MD5Context context;
	unsigned char digest[16];
	char str[33];

	AES_KEY key;
	char *msg = "Hello World";
	char ret[16] = "";

	// test copy in linux
//	copyDir(argv[1], argv[2], X_DECRYPT);
	
	// test md5
	MD5_Init(&context);
	MD5_Update(&context, "Hello World", strlen("Hello World"));
	MD5_Final(&context, digest);
	MD5_Str(digest, str);
	if (strcmp(str, "b10a8db164e0754105b7a99be72e3fe5") == 0)
		printf("MD5 OK\n");
	else 
		printf("MD5 Wrong\n");

	// test aes
    AES_set_encrypt_key("aes key", 128, &key);
	AES_encrypt((unsigned char*)msg, ret, &key);
    AES_set_decrypt_key("aes key", 128, &key);
    AES_decrypt((unsigned char*)ret, ret, &key);
    if (memcmp(msg, ret, strlen(msg)) == 0)
    	printf("AES OK\n");
    else
    	printf("AES WRONG\n");
	// test aes

	//printf("%ld\n", getCTime(argv[1]));
	/*
	// command line 
	// windwos下需要对文件通配符进行处理
	int i = 0, j = 0, filesNum = 0;
	int recursive = 0, update = 0, encrypt = 0, decrypt = 0, check = 0;
	size_t len;
	char key[KEY_LEN] = "";
	char **files = NULL;
	
	// getopt
	for (i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			len = strlen(argv[i]);
			if (len == 1)
				fprintf(stderr, "Bad option : '%s'\n", argv[i]);
			else if (len == 2 && argv[i][1] == '-')
			{
				if (argv[i][1] == '-')
					continue;
				else
				{

				}
			}
				continue;
			else if (argv[i][1] == '-')
			{
				if (strcmp(argv[i], "--recursive") == 0)
				{

				}
				else if (strcmp(argv[i], "--update") == 0)
				{

				}
				else if (strcmp(argv[i], "--encrypt") == 0)
				{

				}
				else if (strcmp(argv[i], "--decrypt") == 0)
				{

				}
				else if (strcmp(argv[i], "--check") == 0)
				{

				}
			}
			else
			{
				for (j = 1; j < len; ++j)
				{
					switch (argv[i][j])
					{
					case 'r':
						break;
					case 'u':
						break;
					case 'e':
						break;
					case 'd':
						break;
					case 'c':
						break;
					}
				}
			}
		}
		else
		{
			filesNum = argc - i;
			files = filesNum < 1 ? NULL : &argv[i];
			break;	
		}

		if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--recursive") == 0)
			recursive = 1;
		else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--update") == 0)
			update = 1;
		else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--encrypt") == 0)
		{
			encrypt = 1;
			decrypt = 0;
			++i;
			if (i >= argc)
			{
				fprintf(stderr, "Missing encrypt key.\n");
				return 0;
			}
			if (strlen(argv[i]) > KEY_LEN)
			{
				fprintf(stderr, "Encrypt key is too long.");
				return 0;
			}
			strcpy(key, argv[i]);
		}
		else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "-decrypt") == 0)
		{
			decrypt = 1;
			encrypt = 0;
			++i;
			if (i >= argc)
			{
				fprintf(stderr, "Missing decrypt key.\n");
				return 0;
			}
			if (strlen(argv[i]) > KEY_LEN)
			{
				fprintf(stderr, "Decrypt key is too long.");
				return 0;
			}
			strcpy(key, argv[i]);
		}
		else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--check") == 0)
			check = 1;
		else if (strcmp(argv[i], "--") == 0)
		{
			filesNum = argc - i - 1;
			if (filesNum < 1)
				files = NULL;
			else
				files = &argv[i + 1];
		}
		else if (argv[i][0] == '-')
		{
			fprintf(stderr, "Illegal option %s\n", argv[i]);
			return 0;
		}
		else
		{
			
		}
	}

	printf("recursive = %d, update = %d, encrypt = %d, decrypt = %d\n", recursive, update, encrypt, decrypt);
	printf("%s\n", key);
	printf("filesNum = %d\n", filesNum);
	for (i = 0; i < filesNum; ++i)
		printf("%s\n", files[i]);
*/
	return 0;
}