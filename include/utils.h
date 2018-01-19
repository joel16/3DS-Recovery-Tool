#ifndef UTILS_H
#define UTILS_H

#include <3ds.h>

#define touchInRect(x1, y1, x2, y2) ((touchGetX() >= (x1) && touchGetX() <= (x2)) && (touchGetY() >= (y1) && touchGetY() <= (y2)))

bool darkTheme;

Result Utils_SaveConfig(bool dark_theme);
Result Utils_LoadConfig(void);
bool Utils_IsN3DS(void);
u16 touchGetX(void);
u16 touchGetY(void);

#endif