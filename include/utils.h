#ifndef UTILS_H
#define UTILS_H

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void getSizeString(char *string, uint64_t size);
bool isN3DS(void);
void utf2ascii(char* dst, u16* src);
char * base64Encode(u8 const * bytesToEnc, size_t bufLen);

#endif