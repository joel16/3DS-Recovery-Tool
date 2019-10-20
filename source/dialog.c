#include <3ds.h>

#include "c2d_helper.h"
#include "utils.h"

Result Dialog_Draw(const char *top_message, const char *bottom_message) {
	Result ret = 0;
	
	float top_width = Draw_GetTextWidth(0.6f, top_message);
	float top_height = Draw_GetTextHeight(0.6f, top_message);
	
	float bottom_width = Draw_GetTextWidth(0.6f, bottom_message);
	float bottom_height = Draw_GetTextHeight(0.6f, bottom_message);
	
	touchPosition touch;
	
	while (aptMainLoop()) {
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_SceneBegin(RENDER_TOP);
		Draw_Rect(0, 0, 400, 240, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		Draw_Text(((400 - top_width) / 2), (((240 - top_height) / 2) - 20), 0.6f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, top_message);
		Draw_Text(((400 - bottom_width) / 2), (((240 - bottom_height) / 2) + 20), 0.6f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, bottom_message);

		C2D_SceneBegin(RENDER_BOTTOM);
		Draw_Rect(0, 0, 320, 240, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		Draw_Text(((160 - Draw_GetTextWidth(0.5f, "CANCEL")) / 2), (220 - Draw_GetTextHeight(0.5f, "CANCEL")), 0.5f, C2D_Color32(0, 151, 136, 255), "CANCEL");
		Draw_Text((((160 - Draw_GetTextWidth(0.5f, "OK")) / 2) + 160), (220 - Draw_GetTextHeight(0.5f, "OK")), 0.5f, C2D_Color32(0, 151, 136, 255), "OK");
		
		Draw_EndFrame();

		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();
		
		if ((kDown & KEY_A) || ((touchInRect(160, 190, 320, 240)) && (kDown & KEY_TOUCH))) {
			ret = 1;
			break;
		}
		else if ((kDown & KEY_B) || ((touchInRect(0, 190, 159, 240)) && (kDown & KEY_TOUCH))) {
			ret = -1;
			break;
		}
	}
	
	return ret;
}
