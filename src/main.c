#include "xcp.h"
#include "func.h"
#include "md5.h"

#if defined(XCP_WIN)
#include "winopt.h"
#else 
#include <getopt.h>
#endif


int main(int argc, char **argv)
{
#if defined (XCP_WIN)
	WIN32_FIND_DATA fd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	TCHAR tmp_t[PATH_MAX];
	char tmp[PATH_MAX];
#endif
	int opt, i;
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

	int pathnum = 0;
	const char **path = (char **)malloc(argc * sizeof(char*));

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
			MD5(optarg, digest);
			break;
		default:
			fprintf(stderr, "Bad Option\n");
			break;
		}
	}
	x_kind |= x_update;
	for (i = 1; i < optind; ++i)
	{
		if (argv[i][0] != '-' && strcmp(argv[i - 1], "--key") != 0)
		{
			path[pathnum++] = argv[i];
		}
	}
	for (i = optind; i < argc; ++i)
		path[pathnum++] = argv[i];

#if defined (XCP_WIN)
	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	{
		for (i = 0; i < pathnum; ++i)
		{
			c2t(path[i], tmp_t);
			if (PathIsDirectory(tmp_t))
				xcp(path[i], NULL, x_kind, digest);
			else
			{
				hFind = FindFirstFile(tmp_t, &fd);
				while (hFind != INVALID_HANDLE_VALUE)
				{
					t2c(fd.cFileName, tmp);
					xcp(tmp, NULL, x_kind, digest);
					if (!FindNextFile(hFind, &fd))
					{
						FindClose(hFind);
						hFind = INVALID_HANDLE_VALUE;
					}
				}
			}
		}	
	}
	else if (pathnum >= 2)
	{
		for (i = 0; i < pathnum - 1; ++i)
		{
			c2t(path[i], tmp_t);
			if (PathIsDirectory(tmp_t))
				xcp(path[i], path[pathnum - 1], x_kind, digest);
			else
			{
				hFind = FindFirstFile(tmp_t, &fd);
				while (hFind != INVALID_HANDLE_VALUE)
				{
					t2c(fd.cFileName, tmp);
					xcp(tmp, path[pathnum - 1], x_kind, digest);
					if (!FindNextFile(hFind, &fd))
					{
						FindClose(hFind);
						hFind = INVALID_HANDLE_VALUE;
					}
				}
			}
		}
	}
#else
	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	{
		for (i = 0; i < pathnum; ++i)
			xcp(path[i], NULL, x_kind, digest);
	}
	else if (pathnum >= 2)
	{
		for (i = 0; i < pathnum - 1; ++i)
			xcp(path[i], path[pathnum - 1], x_kind, digest);
	}
#endif
	else
		fprintf(stderr, "Missing Dest Fold!");
	

/*
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
*/

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
