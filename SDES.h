#ifndef SDES_H
#define SDES_H

#include <stdint.h>

uint8_t encrypt(uint8_t b, uint16_t key);
uint8_t decrypt(uint8_t b, uint16_t key);
void decryptString(uint8_t *b, char *dst, uint16_t key);
void encryptString(uint8_t *b, char *dst, uint16_t key);

#endif