#ifndef _3DS_RECOVERY_TOOL_TEXTURES_H
#define _3DS_RECOVERY_TOOL_TEXTURES_H

C2D_Image icon_toggle_on, icon_toggle_off, icon_back, icon_wifi_0, icon_wifi_1, icon_wifi_2, icon_wifi_3, \
	battery_20, battery_20_charging, battery_30, battery_30_charging, battery_50, battery_50_charging, \
	battery_60, battery_60_charging, battery_80, battery_80_charging, battery_90, battery_90_charging, \
	battery_full, battery_full_charging, battery_low, battery_unknown;

void Textures_Load(void);
void Textures_Free(void);

#endif
