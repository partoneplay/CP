#ifndef AES_H
#define AES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ ((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
#define PUTU32(ct, st) { (ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); (ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }

#define MAXKC   (256/32)
#define MAXKB   (256/8)
#define MAXNR   14

// Because array size can't be a const in C, the following two are macros. Both sizes are in bytes.
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

#ifdef  __cplusplus
extern "C" {
#endif

// This should be a hidden type, but EVP requires that the size be known
struct aes_key_st {
    unsigned int rd_key[4 *(AES_MAXNR + 1)];
    int rounds;
};
typedef struct aes_key_st AES_KEY;

int AES_set_encrypt_key(const unsigned char *userKey, const int bits, AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits, AES_KEY *key);

/* This controls loop-unrolling in aes_core.c */
#undef FULL_UNROLL

void AES_encrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key);
void AES_decrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key);

#ifdef  __cplusplus
}
#endif

#endif /* !AES_H */
