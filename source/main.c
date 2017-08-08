#include <3ds.h>

#include <setjmp.h>

#include <string.h>
#include <stdio.h>

#include "cfg.h"
#include "clock.h"
#include "fs.h"
#include "utils.h"
#include "screen.h"

#define selector_yDistance 30

jmp_buf exitJmp;

void initServices(void)
{
	cfguInit();
	fsInit();
	sdmcInit();
	openArchive(ARCHIVE_SDMC);
	
	gfxInitDefault();
	gfxSet3D(false);
	romfsInit();
	screen_init();
	
	if (isN3DS())
		osSetSpeedupEnable(true);
}

void termServices(void)
{
	osSetSpeedupEnable(0);
	
	screen_exit();
	romfsExit();
	gfxExit();
	
	closeArchive();
	sdmcExit();
	fsExit();
}

void mainMenu(void);

void restoreMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 3;
	
	//Result ret = 0;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Restore");
		
		screen_draw_rect(0, selector_image_y, 400, 30, RGBA8(230, 232, 232, 255));
		
		screen_draw_string(10, 65, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Restore LocalFriendCodeSeed");
		screen_draw_string(10, 125, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Restore SecureInfo");
		
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		screen_select(GFX_BOTTOM);
		screen_end_frame();
		
		if (kDown & KEY_DDOWN)
			selection++;
		else if (kDown & KEY_DUP)
			selection--;
		
		if (selection > max_items) 
			selection = 1;
		if (selection < 1) 
			selection = max_items;
		
		if (kDown & KEY_A)
		{
			switch(selection)
			{
				case 1:
					mainMenu();
				case 2:
					// ret = CFGI_RestoreNANDLocalFriendCodeSeed();
					selection = 1;
					break;
				case 3:
					// ret = CFGI_RestoreNANDSecureInfo();
					selection = 1;
					break;
			}
		}
	}
}

void mainMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 3;
	
	screen_clear(GFX_TOP, RGBA8(250, 250, 250, 255));
	screen_clear(GFX_BOTTOM, RGBA8(250, 250, 250, 255));
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "3DS Tool");
		
		screen_draw_rect(0, selector_image_y, 400, 30, RGBA8(230, 232, 232, 255));
		
		screen_draw_string(10, 65, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Back-up");
		screen_draw_string(10, 95, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Restore");
		screen_draw_string(10, 125, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Exit");
		
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		screen_select(GFX_BOTTOM);
		screen_end_frame();
		
		if (kDown & KEY_DDOWN)
			selection++;
		else if (kDown & KEY_DUP)
			selection--;
		
		if (selection > max_items) 
			selection = 1;
		if (selection < 1) 
			selection = max_items;
		
		if (kDown & KEY_A)
		{
			switch(selection)
			{
				case 1:
					break;
				case 2:
					restoreMenu();
					break;
				case 3:
					longjmp(exitJmp, 1);
					break;
			}
		}
	}
}

int main(int argc, char **argv) 
{
	initServices();
	
	if(setjmp(exitJmp)) 
	{
		termServices();
		return 0;
	}
	
	mainMenu();
	
	termServices();
	
	return 0;
}