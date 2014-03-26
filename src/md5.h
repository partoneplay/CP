#ifndef MD5_H
#define MD5_H

#include "os.h"

typedef struct
{
	unsigned int state[4];
	unsigned int count[2];
	unsigned char buffer[64];
} MD5Context;

#ifdef  __cplusplus
extern "C" {
#endif

	void MD5(const char *str, unsigned char digest[16]);
	void MD5_Init(MD5Context *context);
	void MD5_Update(MD5Context *context, const unsigned char *buf, int len);
	void MD5_Final(MD5Context *context, unsigned char digest[16]);
	int MD5_File(const char *filename, unsigned char digest[16], size_t offset, const unsigned char *head);
	void MD5_Str(const unsigned char digest[16], char str[33]);

#ifdef  __cplusplus
}
#endif

#endif