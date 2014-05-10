#include "crypt.h"
#include "os.h"
#include "md5.h"

#include "aes.h"
#include "des.h"

/* function pointer
void (*crypt_set_encrypt_key)(const char *pkey, CRYPT_KEY *key);
void (*crypt_set_decrypt_key)(const char *pkey, CRYPT_KEY *key);
void (*crypt_freekey)(CRYPT_KEY key);
void (*crypt_encrypt)(unsigned char *in, unsigned char *out, CRYPT_KEY key);
void (*crypt_decrypt)(unsigned char *in, unsigned char *out, CRYPT_KEY key);
*/


// AES
void aes_set_encrypt_key(const char *pkey, CRYPT_KEY *key)
{
	*key = (AES_KEY*)malloc(sizeof(AES_KEY));
	AES_set_encrypt_key(pkey, 128, *key);
}
void aes_set_decrypt_key(const char *pkey, CRYPT_KEY *key)
{
	*key = (AES_KEY*)malloc(sizeof(AES_KEY));
	AES_set_decrypt_key(pkey, 128, *key);
}
void aes_freekey(CRYPT_KEY key)
{
	free((AES_KEY*)key);
}
void aes_encrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key)
{
	AES_encrypt(in, out, (AES_KEY*)key);
}
void aes_decrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key)
{
	AES_decrypt(in, out, (AES_KEY*)key);
}


// DES
void des_set_encrypt_key(const char *pkey, CRYPT_KEY *key)
{
	*key = (DES_KEY*)malloc(sizeof(DES_KEY));
	DES_set_key(pkey, *key);
}
void des_set_decrypt_key(const char *pkey, CRYPT_KEY *key)
{
	*key = (DES_KEY*)malloc(sizeof(DES_KEY));
	DES_set_key(pkey, *key);
}
void des_freekey(CRYPT_KEY key)
{
	free((DES_KEY*)key);
}
void des_encrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key)
{
	DES_encrypt(in, out, (DES_KEY*)key);
}
void des_decrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key)
{
	DES_decrypt(in, out, (DES_KEY*)key);
}