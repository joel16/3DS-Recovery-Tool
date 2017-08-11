#include <3ds.h>

#include <setjmp.h>

#include <string.h>
#include <stdio.h>

#include "am.h"
#include "cfg.h"
#include "clock.h"
#include "compile_date.h"
#include "fs.h"
#include "utils.h"
#include "screen.h"

#define selector_yDistance 30

jmp_buf exitJmp;

void initServices(void)
{
	_amInit();
	cfgiInit();
	fsInit();
	sdmcInit();
	openArchive(ARCHIVE_SDMC);
	
	gfxInitDefault();
	gfxSet3D(false);
	romfsInit();
	screen_init();
	
	if (isN3DS())
		osSetSpeedupEnable(true);
	
	//makeDir();
	//makeDir();
	//makeDir();
}

void termServices(void)
{
	osSetSpeedupEnable(0);
	
	screen_exit();
	romfsExit();
	gfxExit();
	
	closeArchive(ARCHIVE_SDMC);
	sdmcExit();
	fsExit();
	cfgiExit();
	_amExit();
}

void mainMenu(void);

void backupMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 3;
	
	Result res = 0;
	
	char func[20];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Backup");
		
		screen_draw_rect(0, selector_image_y, 400, 30, RGBA8(230, 232, 232, 255));
		
		screen_draw_string(10, 65, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Backup LocalFriendCodeSeed");
		screen_draw_string(10, 125, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Backup SecureInfo");
		
		hidScanInput();

		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		
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
					res = copy_file_archive("/rw/sys/LocalFriendCodeSeed_B", "/3ds/LocalFriendCodeSeed_B");
					snprintf(func, 20, "LocalFriendCodeSeed");
					selection = 1;
					isSelected = true;
					break;
				case 3:
					res = copy_file_archive("/rw/sys/SecureInfo_A", "/3ds/SecureInfo_A");
					snprintf(func, 11, "SecureInfo");
					selection = 1;
					isSelected = true;
					break;
			}
		}
		
		else if (kDown & KEY_B)
			mainMenu();
		
		if (R_FAILED(res))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Backup %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "%s backed-up successfully.", func);
		
		screen_end_frame();
	}
}

void restoreMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 3;
	
	Result res = 0;
	
	char func[20];
	
	bool isSelected = false;
	
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

		screen_select(GFX_BOTTOM);
		
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
					res = CFGI_RestoreNANDLocalFriendCodeSeed();
					snprintf(func, 20, "LocalFriendCodeSeed");
					selection = 1;
					isSelected = true;
					break;
				case 3:
					res = CFGI_RestoreNANDSecureInfo();
					snprintf(func, 11, "SecureInfo");
					selection = 1;
					isSelected = true;
					break;
			}
		}
		
		else if (kDown & KEY_B)
			mainMenu();
		
		if (R_FAILED(res))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Restore %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "%s restored successfully.", func);
		
		screen_end_frame();
	}
}

void advancedWipe(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 5;
	
	Result res = 0;
	
	char func[17];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Advanced Wipe");
		
		screen_draw_rect(0, selector_image_y, 400, 30, RGBA8(230, 232, 232, 255));
		
		screen_draw_string(10, 65, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Wipe all temporary titles");
		screen_draw_string(10, 125, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Wipe all expired titles");
		screen_draw_string(10, 155, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Wipe all TWL titles");
		screen_draw_string(10, 185, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Wipe config");
		
		hidScanInput();

		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		
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
					res = AM_DeleteAllTemporaryTitles();
					snprintf(func, 17, "temporary titles");
					selection = 1;
					isSelected = true;
					break;
				case 3:
					res = AM_DeleteAllExpiredTitles(MEDIATYPE_SD);
					snprintf(func, 15, "expired titles");
					selection = 1;
					isSelected = true;
					break;
				case 4:
					res = AM_DeleteAllTwlTitles();
					snprintf(func, 11, "TWL titles");
					selection = 1;
					isSelected = true;
					break;
				case 5:
					res = CFGI_FormatConfig();
					snprintf(func, 7, "config");
					selection = 1;
					isSelected = true;
					break;
			}
		}
		
		else if (kDown & KEY_B)
			mainMenu();
		
		if (R_FAILED(res))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Wipe %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Wiped %s successfully.", func);
		
		screen_end_frame();
	}
}

void miscMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 3;
	
	Result res = 0;
	
	char func[20];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Miscellaneous");
		
		screen_draw_rect(0, selector_image_y, 400, 30, RGBA8(230, 232, 232, 255));
		
		screen_draw_string(10, 65, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Verify sig LocalFriendCodeSeed");
		screen_draw_string(10, 125, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Verify sig SecureInfo");
		
		hidScanInput();

		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		
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
					res = CFGI_VerifySigLocalFriendCodeSeed();
					snprintf(func, 20, "LocalFriendCodeSeed");
					selection = 1;
					isSelected = true;
					break;
				case 3:
					res = CFGI_VerifySigSecureInfo();
					snprintf(func, 11, "SecureInfo");
					selection = 1;
					isSelected = true;
					break;
			}
		}
		
		else if (kDown & KEY_B)
			mainMenu();
		
		if (R_FAILED(res))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Verify %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Verified %s successfully.", func);
		
		screen_end_frame();
	}
}

void mainMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 5;
	
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
		screen_draw_string(10, 125, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Advanced wipe");
		screen_draw_string(10, 155, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Misc");
		screen_draw_string(10, 185, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "Exit");
		
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		screen_select(GFX_BOTTOM);
		screen_draw_stringf(2, 225, 0.41f, 0.41f, RGBA8(54, 54, 54, 255), "3DS Tool v%i.%i0 - %d%02d%02d", VERSION_MAJOR, VERSION_MINOR, YEAR, MONTH, DAY);
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
					backupMenu();
					break;
				case 2:
					restoreMenu();
					break;
				case 3:
					advancedWipe();
					break;
				case 4:
					miscMenu();
					break;
				case 5:
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