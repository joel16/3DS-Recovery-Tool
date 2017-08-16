#ifndef UTILS_H
#define UTILS_H

#include <3ds.h>

#define touchInRect(x1, y1, x2, y2) ((touchGetX() >= (x1) && touchGetX() <= (x2)) && (touchGetY() >= (y1) && touchGetY() <= (y2)))

bool darkTheme;

bool isN3DS(void);
Result setConfig(const char * path, bool set);
u16 touchGetX(void);
u16 touchGetY(void);

#endif