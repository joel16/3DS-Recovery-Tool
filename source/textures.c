#include "c2d_helper.h"
#include "sprites.h"
#include "textures.h"

static C2D_SpriteSheet spritesheet;

void Textures_Load(void) {
	spritesheet = C2D_SpriteSheetLoad("romfs:/res/drawable/sprites.t3x");
    icon_toggle_on = C2D_SpriteSheetGetImage(spritesheet, sprites_btn_material_light_toggle_on_normal_idx);
	icon_toggle_off = C2D_SpriteSheetGetImage(spritesheet, sprites_btn_material_light_toggle_off_normal_idx);
    icon_back = C2D_SpriteSheetGetImage(spritesheet, sprites_ic_arrow_back_normal_idx);
	icon_wifi_0 = C2D_SpriteSheetGetImage(spritesheet, sprites_stat_sys_wifi_signal_0_idx);
	icon_wifi_1 = C2D_SpriteSheetGetImage(spritesheet, sprites_stat_sys_wifi_signal_1_idx);
	icon_wifi_2 = C2D_SpriteSheetGetImage(spritesheet, sprites_stat_sys_wifi_signal_2_idx);
	icon_wifi_3 = C2D_SpriteSheetGetImage(spritesheet, sprites_stat_sys_wifi_signal_3_idx);
	battery_20 = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_20_idx);
	battery_20_charging = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_20_charging_idx);
	battery_30 = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_30_idx);
	battery_30_charging = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_30_charging_idx);
	battery_50 = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_50_idx);
	battery_50_charging = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_50_charging_idx);
	battery_60 = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_60_idx);
	battery_60_charging = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_60_charging_idx);
	battery_80 = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_80_idx);
	battery_80_charging = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_80_charging_idx);
	battery_90 = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_90_idx);
	battery_90_charging = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_90_charging_idx);
	battery_full = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_full_idx);
	battery_full_charging = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_full_charging_idx);
	battery_low = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_low_idx);
	battery_unknown = C2D_SpriteSheetGetImage(spritesheet, sprites_battery_unknown_idx);
}

void Textures_Free(void) {
	C2D_SpriteSheetFree(spritesheet);
}
