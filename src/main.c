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
	int x_kind = X_NONE, x_update = X_NONE, x_view = X_NONE;
	int pathnum = 0;
	const char **path = (const char **)malloc(argc * sizeof(char*));
	unsigned char digest[16] = "pkey";
	unsigned char pkey[33] = "pkey";
	char* const short_options = "vuedcmk:";
	struct option long_options[] = {
		{ "view", no_argument, NULL, 'v' },
		{ "update", no_argument, NULL, 'u' },
		{ "encrypt", no_argument, NULL, 'e' },
		{ "decrypt", no_argument, NULL, 'd' },
		{ "check", no_argument, NULL, 'c' },
		{ "md5sum", no_argument, NULL, 'm' },
		{ "key", required_argument, NULL, 'k' },
		{ 0, 0, 0, 0 }
	};

	while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
	{
		switch (opt)
		{
		case 'v':
			x_view = X_VIEW;
			break;
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
			MD5_Str(digest, pkey);
			break;
		default:
			fprintf(stderr, "Bad Option\n");
			break;
		}
	}
	x_kind |= x_view;
	x_kind |= x_update;
	for (i = 1; i < optind; ++i)
		if (argv[i][0] != '-' && strcmp(argv[i - 1], "--key") != 0)
			path[pathnum++] = argv[i];
	for (i = optind; i < argc; ++i)
		path[pathnum++] = argv[i];

#if defined (XCP_WIN)
	// deal with File Wildcards in windows
	if (x_kind & X_CHECK || x_kind & X_MD5SUM)
	{
		for (i = 0; i < pathnum; ++i)
		{
			c2t(path[i], tmp_t);
			if (PathIsDirectory(tmp_t))
				xcp(path[i], NULL, x_kind, pkey);
			else
			{
				hFind = FindFirstFile(tmp_t, &fd);
				while (hFind != INVALID_HANDLE_VALUE)
				{
					t2c(fd.cFileName, tmp);
					xcp(tmp, NULL, x_kind, pkey);
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
				xcp(path[i], path[pathnum - 1], x_kind, pkey);
			else
			{
				hFind = FindFirstFile(tmp_t, &fd);
				while (hFind != INVALID_HANDLE_VALUE)
				{
					t2c(fd.cFileName, tmp);
					xcp(tmp, path[pathnum - 1], x_kind, pkey);
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
		for (i = 0; i < pathnum; ++i)
			xcp(path[i], NULL, x_kind, pkey);
	else if (pathnum >= 2)
		for (i = 0; i < pathnum - 1; ++i)
			xcp(path[i], path[pathnum - 1], x_kind, pkey);
#endif
	else
		fprintf(stderr, "Missing Dest Fold!\n");
	
	return 0;
}
