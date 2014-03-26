#include "xcp.h"


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
/*	int i, x_kind = X_NONE, x_update = X_NONE;
	unsigned char *key = NULL;

	int pathnum = 0;
	char **path;

	for (i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--update") == 0)
			x_update = X_UPDATE;
		else if (strcmp(argv[i], "--encrypt") == 0 && argc > (i + 1))
		{
			x_kind = X_ENCRYPT;
			key = argv[++i];
			++i;
			break;
		}
		else if (strcmp(argv[i], "--decrypt") == 0 && argc > (i + 1))
		{
			x_kind = X_DECRYPT;
			key = argv[++i];
			++i;
			break;
		}
		else if (strcmp(argv[i], "--check") == 0 && argc > (i + 1))
		{
			x_kind = X_CHECK;
			key = argv[++i];
			++i;
			break;
		}
		else if (strcmp(argv[i], "--md5sum") == 0)
		{
			x_kind = X_MD5SUM;
			++i;
			break;
		}
		else if (strcmp(argv[i], "--") == 0)
		{
			++i;
			break;
		}
		else if (argv[i][0] == '-')
		{
			fprintf(stderr, "Bad Option\n");
			return;
		}
		else
			break;
	}
	if (key != NULL)
		key = strrchr(key, '=') + 1;
	x_kind |= x_update;

	path = &argv[i];
	pathnum = argc - i;
	
	for (i = 0; i < pathnum - 1; ++i)
		xcpDir(path[i], path[pathnum - 1], x_kind, key);
	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
		xcpDir(path[pathnum - 1], NULL, x_kind, key);
	*/
/*	xcpDir("a", "b", X_ENCRYPT, (unsigned char*)"partoneplay");
	xcpDir("a", NULL, X_CHECK, (unsigned char*)"partoneplay");
	xcpDir("b", "c", X_DECRYPT, (unsigned char*)"partoneplay");
	xcpDir("a", NULL, X_MD5SUM, NULL);
	xcpDir("c", NULL, X_MD5SUM, NULL);
	*/
	TCHAR tch;
	LPCTSTR lp;

	return 0;
}