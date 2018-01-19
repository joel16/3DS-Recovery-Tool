#include "colours.h"
#include "dialog.h"
#include "pp2d.h"
#include "utils.h"

Result Dialog_Draw(const char * topMessage, const char * bottomMessage)
{
	Result ret = 0;
	
	float topWidth = pp2d_get_text_width(topMessage, 0.6f, 0.6f);
	float topHeight = pp2d_get_text_height(topMessage, 0.6f, 0.6f);
	
	float bottomWidth = pp2d_get_text_width(bottomMessage, 0.6f, 0.6f);
	float bottomHeight = pp2d_get_text_height(bottomMessage, 0.6f, 0.6f);
	
	touchPosition touch;
	
	while (aptMainLoop())
	{
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);

			pp2d_draw_rectangle(0, 0, 400, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			pp2d_draw_text(((400 - topWidth) / 2), (((240 - topHeight) / 2) - 20), 0.6f, 0.6f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, topMessage);
			pp2d_draw_text(((400 - bottomWidth) / 2), (((240 - bottomHeight) / 2) + 20), 0.6f, 0.6f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, bottomMessage);

		pp2d_end_draw();

		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);

			pp2d_draw_rectangle(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			pp2d_draw_text(((160 - pp2d_get_text_width("CANCEL", 0.5f, 0.5f)) / 2), (220 - pp2d_get_text_height("CANCEL", 0.5f, 0.5f)), 0.5f, 0.5f, RGBA8(0, 151, 136, 255), "CANCEL");
			pp2d_draw_text((((160 - pp2d_get_text_width("OK", 0.5f, 0.5f)) / 2) + 160), (220 - pp2d_get_text_height("OK", 0.5f, 0.5f)), 0.5f, 0.5f, RGBA8(0, 151, 136, 255), "OK");
		
		pp2d_end_draw();

		hidScanInput();
		hidTouchRead(&touch);
		u32 kDown = hidKeysDown();
		
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