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

xcp -u|--recusive 


*/

int main(int argc, char **argv)
{
//	X_encrypt("1.txt", "2.cxc", "Key");
//	X_decrypt("2.cxc", "3.txt", "Key");

//	char digest[16] = "";
//	char ret[33] = "";
//	MD5_File(argv[1], digest, 3, "123");
//	MD5_Str(digest, ret);
//	printf("%s\n", ret);
	
	// command line 
	int i, x_kind = X_NONE;
	char *key;

	int pathnum = 0;
	char **path;

	for (i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--update") == 0)
			x_kind |= X_UPDATE;
		else if (strcmp(argv[i], "--encrypt") == 0 && argc > (i + 1))
		{
			x_kind ^= X_DECRYPT;
			x_kind |= X_ENCRYPT;
			key = argv[++i];
		}
		else if (strcmp(argv[i], "--decrypt") == 0 && argc > (i + 1))
		{
			x_kind ^= X_ENCRYPT;
			x_kind |= X_DECRYPT;
			key = argv[++i];
		}
		else if (strcmp(argv[i], "--check") == 0 && argc > (i + 1))
		{
			x_kind &= X_CHECK;
			key = argv[++i];
		}
		else if (strcmp(argv[i], "--md5sum") == 0)
		{
			x_kind &= X_MD5SUM;
		}
		else if (strcmp(argv[i], "--") == 0)
		{
			++i;
			break;
		}
		else
			break;
	}

	path = &argv[i];
	pathnum = argc - i;

	for (i = 0; i < pathnum - 1; ++i)
	{
		printf("%s to %s\n", path[i], path[pathnum - 1]);
	}

	system("pause");
	return 0;
}