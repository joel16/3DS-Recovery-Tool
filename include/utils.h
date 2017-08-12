#ifndef UTILS_H
#define UTILS_H

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool darkTheme;

void getSizeString(char *string, uint64_t size);
bool isN3DS(void);
void utf2ascii(char* dst, u16* src);
void setConfig(const char * path, bool set);

#endif