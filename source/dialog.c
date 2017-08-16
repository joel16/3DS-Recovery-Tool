#include "colours.h"
#include "dialog.h"
#include "screen.h"
#include "utils.h"

Result drawDialog(const char * topMessage, const char * bottomMessage)
{
	Result ret = 0;
	
	float topWidth = screen_get_string_width(topMessage, 0.6f, 0.6f);
	float topHeight = screen_get_string_height(topMessage, 0.6f, 0.6f);
	
	float bottomWidth = screen_get_string_width(bottomMessage, 0.6f, 0.6f);
	float bottomHeight = screen_get_string_height(bottomMessage, 0.6f, 0.6f);
	
	touchPosition touch;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		
		screen_select(GFX_TOP);
		screen_draw_rect(0, 0, 400, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		screen_draw_string(((400 - topWidth) / 2), (((240 - topHeight) / 2) - 20), 0.6f, 0.6f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, topMessage);
		screen_draw_string(((400 - bottomWidth) / 2), (((240 - bottomHeight) / 2) + 20), 0.6f, 0.6f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, bottomMessage);
		
		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		screen_draw_rect(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		screen_draw_string(((160 - screen_get_string_width("CANCEL", 0.5f, 0.5f)) / 2), (220 - screen_get_string_height("CANCEL", 0.5f, 0.5f)), 0.5f, 0.5f, RGBA8(0, 151, 136, 255), "CANCEL");
		screen_draw_string((((160 - screen_get_string_width("OK", 0.5f, 0.5f)) / 2) + 160), (220 - screen_get_string_height("OK", 0.5f, 0.5f)), 0.5f, 0.5f, RGBA8(0, 151, 136, 255), "OK");
		
		screen_end_frame();
		
		if ((kDown & KEY_A) || ((touchInRect(160, 190, 320, 240)) && (kDown & KEY_TOUCH)))
		{
			ret = 1;
			break;
		}
		else if ((kDown & KEY_B) || ((touchInRect(0, 190, 159, 240)) && (kDown & KEY_TOUCH)))
		{
			ret = -1;
			break;
		}
	}
	
	return ret;
}