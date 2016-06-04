#ifndef SDES_H
#define SDES_H

BYTE encrypt(BYTE b, WORD key);
BYTE decrypt(BYTE b, WORD key);
void decryptString(BYTE *b, char *dst, WORD key);
void encryptString(BYTE *b, char *dst, WORD key);

#endif