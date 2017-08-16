#ifndef UTILS_H
#define UTILS_H

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool darkTheme;

bool isN3DS(void);
Result setConfig(const char * path, bool set);

#endif