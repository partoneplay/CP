#include "os.h"


int dealExt(char *filename, int x_kind)
{
	size_t len = 0, extlen = 0;
	char buf[NAME_MAX] = "";
	
	len = strlen(filename);
	extlen = strlen(EXT_NAME);

	if (len <= extlen)
		return X_NONE;

	if (x_kind == X_ENCRYPT)
	{
		strcat(filename, EXT_NAME);
		return X_ENCRYPT;
	}
	else if (x_kind == X_DECRYPT)
	{
		// the extname must be EXT_NAME
		strcpy(buf, &filename[len]);
		memcpy(buf, &filename[len - extlen], extlen);
		if (strcmp(buf, EXT_NAME) == 0)
		{
			memset(&filename[len - extlen], 0, extlen);
			return X_DECRYPT;
		}
		else 
			return X_NONE;
	}
	else
		return X_NONE;

	return x_kind;
}