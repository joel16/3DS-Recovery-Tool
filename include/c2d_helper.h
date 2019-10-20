#ifndef _3DS_RECOVERY_TOOL_C2D_HELPER_H
#define _3DS_RECOVERY_TOOL_C2D_HELPER_H

#include <citro2d.h>

#define WHITE                 C2D_Color32(255, 255, 255, 255)
#define BG_COLOUR_LIGHT       C2D_Color32(250, 250, 250, 255)
#define BG_COLOUR_DARK        C2D_Color32(48, 48, 48, 255)
#define SELECTOR_COLOUR_LIGHT C2D_Color32(230, 232, 232, 255)
#define SELECTOR_COLOUR_DARK  C2D_Color32(28, 30, 30, 255)
#define TEXT_COLOUR_LIGHT     C2D_Color32(54, 54, 54, 255)
#define TEXT_COLOUR_DARK      C2D_Color32(230, 230, 230, 255)

extern C3D_RenderTarget *RENDER_TOP, *RENDER_BOTTOM;
extern C2D_TextBuf c2d_static_buf, c2d_dynamic_buf, c2d_size_buf;
extern C2D_Font font;
typedef u32 Colour;

void Draw_EndFrame(void);
void Draw_Text(float x, float y, float size, Colour colour, const char *text);
void Draw_Textf(float x, float y, float size, Colour colour, const char* text, ...);
void Draw_GetTextSize(float size, float *width, float *height, const char *text);
float Draw_GetTextWidth(float size, const char *text);
float Draw_GetTextHeight(float size, const char *text);
bool Draw_Rect(float x, float y, float w, float h, Colour colour);
bool Draw_Image(C2D_Image image, float x, float y);

#endif
