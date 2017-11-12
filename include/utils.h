#ifndef UTILS_H
#define UTILS_H

#include <3ds.h>

#define touchInRect(x1, y1, x2, y2) ((touchGetX() >= (x1) && touchGetX() <= (x2)) && (touchGetY() >= (y1) && touchGetY() <= (y2)))

bool darkTheme;

Result saveConfig(bool dark_theme);
Result loadConfig(void);
void makeDirs(void);
bool isN3DS(void);
u16 touchGetX(void);
u16 touchGetY(void);

#endif