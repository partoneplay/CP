#ifndef CRYPT_H
#define CRYPT_H

#include <stdio.h>

typedef void* CRYPT_KEY;


// AES
void aes_set_encrypt_key(const char *pkey, CRYPT_KEY *key);
void aes_set_decrypt_key(const char *pkey, CRYPT_KEY *key);
void aes_freekey(CRYPT_KEY key);
void aes_encrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key);
void aes_decrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key);


// DES
void des_set_encrypt_key(const char *pkey, CRYPT_KEY *key);
void des_set_decrypt_key(const char *pkey, CRYPT_KEY *key);
void des_freekey(CRYPT_KEY key);
void des_encrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key);
void des_decrypt(unsigned char *in, unsigned char *out, const CRYPT_KEY key);

#endif