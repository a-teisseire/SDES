#include "SDES.h"

#define CSL(c, sh, sz) ((c << sh) | ((c >> (sz - sh)) & ((1 << sz) - 1)))
#define K(c, i, j) (((c & (1 << i)) >> i ) << j)

/*
BYTE S0[] = {
	0xB1, //10 11 00 01
	0x1B, //00 01 10 11
	0xD8, //11 01 10 00
	0xB7  //10 11 01 11
};

BYTE S1[] = {
	0xE4, //11 10 01 00
	0xD2, //11 01 00 10
	0x13, //00 01 00 11
	0xC6  //11 00 01 10
};
*/

DWORD S0 = 0xB7D81BB1;
DWORD S1 = 0xC613D2E4;

WORD P10(WORD k)
{
	return (K(k,7,9)|K(k,5,8)|K(k,8,7)|K(k,3,6)|K(k,6,5)|K(k,0,4)|K(k,9,3)|K(k,1,2)|K(k,2,1)|K(k,4,0)) & 0x3FF;
}

BYTE P8(WORD k)
{
	return (K(k,4,7)|K(k,7,6)|K(k,3,5)|K(k,6,4)|K(k,2,3)|K(k,5,2)|K(k,0,1)|K(k,1,0)) & 0xFF;
}

BYTE P4(BYTE k)
{
	return (K(k,2,3)|K(k,0,2)|K(k,1,1)|K(k,3,0)) & 0xF;
}

WORD shift(WORD c, BYTE shift)
{
	WORD a, b;
	
	a = (c >> 5) & 0x1F;
	b = c & 0x1F;

	return ((CSL(a, shift, 5) & 0x1F) << 5) | (CSL(b, shift, 5) & 0x1F);
}

BYTE K1(WORD key)
{
	return P8(shift(P10(key), 1));
}

BYTE K2(WORD key)
{
	return P8(shift(P10(key), 3));
}

BYTE IP(BYTE d)
{
	return (K(d,6,7)|K(d,2,6)|K(d,5,5)|K(d,7,4)|K(d,4,3)|K(d,0,2)|K(d,3,1)|K(d,1,0)) & 0xFF;
}

BYTE RIP(BYTE d)
{
	return (K(d,4,7)|K(d,7,6)|K(d,5,5)|K(d,3,4)|K(d,1,3)|K(d,6,2)|K(d,0,1)|K(d,2,0)) & 0xFF;
}

BYTE EP(BYTE n)
{
	return (K(n,0,7)|K(n,3,6)|K(n,2,5)|K(n,1,4)|K(n,2,3)|K(n,1,2)|K(n,0,1)|K(n,3,0)) & 0xFF;
}

BYTE SBox(BYTE box, BYTE input)
{
	BYTE row, col;
	
	row = (BYTE)(K(input,3,1)|K(input,0,0));
	col = (BYTE)(K(input,2,1)|K(input,1,0));

	return ((((!box ? S0 : S1) >> (row << 3)) & 0xFF) >> (col << 1)) & 0x3;
}

BYTE FK(BYTE d, BYTE sk)
{
	BYTE l = (d >> 4) & 0xF, r = (d) & 0xF;

	//F Mapping
	BYTE f = (EP(r) ^ sk);
	BYTE fl = (f >> 4) & 0xF, fr = f & 0xF;

	f = P4((SBox(0, fl) << 2) | SBox(1, fr));

	return ((l ^ f & 0xF) << 4) | r; 
}

BYTE SW(BYTE d)
{
	return ((d & 0xF) << 4)|((d >> 4) & 0xF);
}

BYTE encrypt(BYTE b, WORD key)
{
	BYTE k1 = K1(key), k2 = K2(key);

	return RIP(FK(SW(FK(IP(b), k1)), k2));
}

BYTE decrypt(BYTE b, WORD key)
{
	BYTE k1 = K1(key), k2 = K2(key);

	return RIP(FK(SW(FK(IP(b), k2)), k1));
}

void decryptString(BYTE *b, char *dst, WORD key)
{
	while(*b)
		*dst++ = (char)decrypt(*b++, key);

	*dst = 0;
}

void encryptString(BYTE *b, char *dst, WORD key)
{
	while(*b)
		*dst++ = (char)encrypt(*b++, key);

	*dst = 0;
}