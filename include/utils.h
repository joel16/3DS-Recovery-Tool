#ifndef _3DS_RECOVERY_TOOL_UTILS_H
#define _3DS_RECOVERY_TOOL_UTILS_H

#include <3ds.h>

#define touchInRect(x1, y1, x2, y2) ((touchGetX() >= (x1) && touchGetX() <= (x2)) && (touchGetY() >= (y1) && touchGetY() <= (y2)))

extern bool dark_theme;

Result Utils_SaveConfig(bool dark_theme);
Result Utils_LoadConfig(void);
u16 touchGetX(void);
u16 touchGetY(void);
void Utils_SetMax(int *set, int value, int max);
void Utils_SetMin(int *set, int value, int min);

#endif
