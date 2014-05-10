#ifndef DES_H
#define DES_H

void Des_Run(char out[8], char in[8], char kind);
void Des_SetKey(const char pkey[8]);

#endif