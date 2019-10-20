#include <stdarg.h>

#include "c2d_helper.h"

C3D_RenderTarget *RENDER_TOP, *RENDER_BOTTOM;
C2D_TextBuf c2d_static_buf, c2d_dynamic_buf, c2d_size_buf;
C2D_Font font;

void Draw_EndFrame(void) {
	C2D_TextBufClear(c2d_dynamic_buf);
	C2D_TextBufClear(c2d_size_buf);
	C3D_FrameEnd(0);
}

void Draw_Text(float x, float y, float size, Colour colour, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, font, c2d_dynamic_buf, text);
	C2D_TextOptimize(&c2d_text);
	C2D_DrawText(&c2d_text, C2D_WithColor, x, y, 0.5f, size, size, colour);
}

void Draw_Textf(float x, float y, float size, Colour colour, const char* text, ...) {
	char buffer[256];
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, 256, text, args);
	Draw_Text(x, y, size, colour, buffer);
	va_end(args);
}

void Draw_GetTextSize(float size, float *width, float *height, const char *text) {
	C2D_Text c2d_text;
	C2D_TextFontParse(&c2d_text, font, c2d_size_buf, text);
	C2D_TextGetDimensions(&c2d_text, size, size, width, height);
}

float Draw_GetTextWidth(float size, const char *text) {
	float width = 0;
	Draw_GetTextSize(size, &width, NULL, text);
	return width;
}

float Draw_GetTextHeight(float size, const char *text) {
	float height = 0;
	Draw_GetTextSize(size, NULL, &height, text);
	return height;
}

bool Draw_Rect(float x, float y, float w, float h, Colour colour) {
	return C2D_DrawRectSolid(x, y, 0.5f, w, h, colour);
}

bool Draw_Image(C2D_Image image, float x, float y) {
	return C2D_DrawImageAt(image, x, y, 0.5f, NULL, 1.0f, 1.0f);
}
