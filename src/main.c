#include "xcp.h"
#include "xgetopt.h"


int main(int argc, char **argv)
{
	int opt;
	char* const short_options = "uedcmk:";
	struct option long_options[] = {
		{ "update", no_argument, NULL, 'u' },
		{ "encrypt", no_argument, NULL, 'e' },
		{ "decrypt", no_argument, NULL, 'd' },
		{ "check", no_argument, NULL, 'c' },
		{ "md5sum", no_argument, NULL, 'm' },
		{ "key", required_argument, NULL, 'k' },
		{ 0, 0, 0, 0 }
	};

	unsigned char digest[16] = "";
	int x_kind = X_NONE, x_update = X_NONE;

	while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
	{
		switch (opt)
		{
		case 'u':
			x_update = X_UPDATE;
			break;
		case 'e':
			x_kind = X_ENCRYPT;
			break;
		case 'd':
			x_kind = X_DECRYPT;
			break;
		case 'c':
			x_kind = X_CHECK;
			break;
		case 'm':
			x_kind = X_MD5SUM;
			break;
		case 'k':
			x_kind = X_CHECK;
			MD5(optarg, digest);
			break;
		default:
			printf("Path: %s\n", optarg);
			break;
		}
	}

	/*
	int i, x_kind = X_NONE, x_update = X_NONE;
	unsigned char *key = NULL;
	unsigned char digest[16];

	int pathnum = 0;
	char **path;

	#if defined(XCP_WIN)
	TCHAR tpath[PATH_MAX] = _T(""), tpath2[PATH_MAX] = _T("");
	#endif

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
	key = strrchr(key, '=');
	if (key == NULL && x_kind != X_MD5SUM)
	{
	fprintf(stderr, "Missing --key=pkey\n");
	return 0;
	}
	x_kind |= x_update;

	path = &argv[i];
	pathnum = argc - i;

	if (x_kind != X_MD5SUM)
	{
	key++;
	MD5(key, digest);
	}

	#if defined(XCP_WIN)
	c2t(path[pathnum - 1], tpath2);
	for (i = 0; i < pathnum - 1; ++i)
	{
	c2t(path[i], tpath);
	xcp(tpath, tpath2, x_kind, digest);
	}
	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	xcp(tpath2, NULL, x_kind, digest);
	#else
	for (i = 0; i < pathnum - 1; ++i)
	xcp(path[i], path[pathnum - 1], x_kind, digest);
	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	xcp(path[pathnum - 1], NULL, x_kind, digest);
	#endif


	/*	xcp("1.txt", "2", X_ENCRYPT, (unsigned char*)"partoneplay");
	xcp("2.cxc", NULL, X_CHECK, (unsigned char*)"partoneplay");
	xcp("2.cxc", "3.txt", X_DECRYPT, (unsigned char*)"partoneplay");
	xcp("1.txt", NULL, X_MD5SUM, NULL);
	xcp("3.txt", NULL, X_MD5SUM, NULL);
	*/

	/*
	xcp(_T("1.txt"), _T("2"), X_ENCRYPT, (unsigned char*)"partoneplay");
	xcp(_T("2.cxc"), NULL, X_CHECK, (unsigned char*)"partoneplay");
	xcp(_T("2.cxc"), _T("3.txt"), X_DECRYPT, (unsigned char*)"partoneplay");
	xcp(_T("1.txt"), NULL, X_MD5SUM, NULL);
	xcp(_T("3.txt"), NULL, X_MD5SUM, NULL);
	*/

	/*
	xcp("a", "b", X_ENCRYPT, (unsigned char*)"partoneplay");
	xcp("b", NULL, X_CHECK, (unsigned char*)"partoneplay");
	xcp("b", "c", X_DECRYPT, (unsigned char*)"partoneplay");
	xcp("a", NULL, X_MD5SUM, NULL);
	xcp("c", NULL, X_MD5SUM, NULL);
	*/

	/*
	xcp(_T("a"), _T("b"), X_ENCRYPT, (unsigned char*)"partoneplay");
	xcp(_T("b"), NULL, X_CHECK, (unsigned char*)"partoneplay");
	xcp(_T("b"), _T("c"), X_DECRYPT, (unsigned char*)"partoneplay");
	xcp(_T("a"), NULL, X_MD5SUM, NULL);
	xcp(_T("c"), NULL, X_MD5SUM, NULL);
	*/

	return 0;
}