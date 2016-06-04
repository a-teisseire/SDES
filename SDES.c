#include <stdint.h>
#include "SDES.h"

#define CSL(c, sh, sz) ((c << sh) | ((c >> (sz - sh)) & ((1 << sz) - 1)))
#define K(c, i, j) (((c & (1 << i)) >> i ) << j)

/*
uint8_t S0[] = {
	0xB1, //10 11 00 01
	0x1B, //00 01 10 11
	0xD8, //11 01 10 00
	0xB7  //10 11 01 11
};

uint8_t S1[] = {
	0xE4, //11 10 01 00
	0xD2, //11 01 00 10
	0x13, //00 01 00 11
	0xC6  //11 00 01 10
};
*/

uint32_t S0 = 0xB7D81BB1;
uint32_t S1 = 0xC613D2E4;

uint16_t P10(uint16_t k)
{
	return (K(k,7,9)|K(k,5,8)|K(k,8,7)|K(k,3,6)|K(k,6,5)|K(k,0,4)|K(k,9,3)|K(k,1,2)|K(k,2,1)|K(k,4,0)) & 0x3FF;
}

uint8_t P8(uint16_t k)
{
	return (K(k,4,7)|K(k,7,6)|K(k,3,5)|K(k,6,4)|K(k,2,3)|K(k,5,2)|K(k,0,1)|K(k,1,0)) & 0xFF;
}

uint8_t P4(uint8_t k)
{
	return (K(k,2,3)|K(k,0,2)|K(k,1,1)|K(k,3,0)) & 0xF;
}

uint16_t shift(uint16_t c, uint8_t shift)
{
	uint16_t a, b;
	
	a = (c >> 5) & 0x1F;
	b = c & 0x1F;

	return ((CSL(a, shift, 5) & 0x1F) << 5) | (CSL(b, shift, 5) & 0x1F);
}

uint8_t K1(uint16_t key)
{
	return P8(shift(P10(key), 1));
}

uint8_t K2(uint16_t key)
{
	return P8(shift(P10(key), 3));
}

uint8_t IP(uint8_t d)
{
	return (K(d,6,7)|K(d,2,6)|K(d,5,5)|K(d,7,4)|K(d,4,3)|K(d,0,2)|K(d,3,1)|K(d,1,0)) & 0xFF;
}

uint8_t RIP(uint8_t d)
{
	return (K(d,4,7)|K(d,7,6)|K(d,5,5)|K(d,3,4)|K(d,1,3)|K(d,6,2)|K(d,0,1)|K(d,2,0)) & 0xFF;
}

uint8_t EP(uint8_t n)
{
	return (K(n,0,7)|K(n,3,6)|K(n,2,5)|K(n,1,4)|K(n,2,3)|K(n,1,2)|K(n,0,1)|K(n,3,0)) & 0xFF;
}

uint8_t SBox(uint8_t box, uint8_t input)
{
	uint8_t row, col;
	
	row = (uint8_t)(K(input,3,1)|K(input,0,0));
	col = (uint8_t)(K(input,2,1)|K(input,1,0));

	return ((((!box ? S0 : S1) >> (row << 3)) & 0xFF) >> (col << 1)) & 0x3;
}

uint8_t FK(uint8_t d, uint8_t sk)
{
	uint8_t l = (d >> 4) & 0xF, r = (d) & 0xF;

	//F Mapping
	uint8_t f = (EP(r) ^ sk);
	uint8_t fl = (f >> 4) & 0xF, fr = f & 0xF;

	f = P4((SBox(0, fl) << 2) | SBox(1, fr));

	return ((l ^ f & 0xF) << 4) | r; 
}

uint8_t SW(uint8_t d)
{
	return ((d & 0xF) << 4)|((d >> 4) & 0xF);
}

uint8_t encrypt(uint8_t b, uint16_t key)
{
	uint8_t k1 = K1(key), k2 = K2(key);

	return RIP(FK(SW(FK(IP(b), k1)), k2));
}

uint8_t decrypt(uint8_t b, uint16_t key)
{
	uint8_t k1 = K1(key), k2 = K2(key);

	return RIP(FK(SW(FK(IP(b), k2)), k1));
}

void decryptString(uint8_t *b, char *dst, uint16_t key)
{
	while(*b)
		*dst++ = (char)decrypt(*b++, key);

	*dst = 0;
}

void encryptString(uint8_t *b, char *dst, uint16_t key)
{
	while(*b)
		*dst++ = (char)encrypt(*b++, key);

	*dst = 0;
}