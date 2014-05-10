#ifndef DES_H
#define DES_H

typedef struct
{
	unsigned char key[16][48];
} DES_KEY;

void DES_set_key(const unsigned char *pkey, DES_KEY *key);
void DES_encrypt(unsigned char *in, unsigned char *out, const DES_KEY *key);
void DES_decrypt(unsigned char *in, unsigned char *out, const DES_KEY *key);

#endif