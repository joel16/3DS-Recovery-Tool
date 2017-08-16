#include <3ds.h>

#include <setjmp.h>

#include <string.h>
#include <stdio.h>

#include "am.h"
#include "cfg.h"
#include "clock.h"
#include "colours.h"
#include "compile_date.h"
#include "fs.h"
#include "utils.h"
#include "screen.h"

#define selector_yDistance 30

#define TEXTURE_TOGGLE_ON  0
#define TEXTURE_TOGGLE_OFF 1

jmp_buf exitJmp;

void initServices(void)
{
	amGetServiceHandle();
	cfgiInit();
	fsInit();
	sdmcInit();
	openArchive(ARCHIVE_SDMC);
	
	gfxInitDefault();
	gfxSet3D(false);
	romfsInit();
	screen_init();
	
	screen_load_texture_png(TEXTURE_TOGGLE_ON, "romfs:/toggleOn.png", true);
	screen_load_texture_png(TEXTURE_TOGGLE_OFF, "romfs:/toggleOff.png", true);
	
	if (isN3DS())
		osSetSpeedupEnable(true);
	
	if (!(dirExists(fsArchive, "/3ds/")))
		makeDir(fsArchive, "/3ds");
	if (!(dirExists(fsArchive, "/3ds/data/")))
		makeDir(fsArchive, "/3ds/data");
	if (!(dirExists(fsArchive, "/3ds/data/3dstool/")))
		makeDir(fsArchive, "/3ds/data/3dstool");
	if (!(dirExists(fsArchive, "/3ds/data/3dstool/backups/")))
		makeDir(fsArchive, "/3ds/data/3dstool/backups");
	if (!(dirExists(fsArchive, "/3ds/data/3dstool/backups/nand/")))
	{
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/ro");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/rw");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/ro/sys");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/rw/sys");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/private");
	}
	
	if (fileExists(fsArchive, "/3ds/data/3dstool/darkTheme.bin"))
	{
		int info = 0;
		
		FILE * read = fopen("/3ds/data/3dstool/darkTheme.bin", "r");
		fscanf(read, "%d", &info);
		fclose(read);
		
		if (info == 0)
			darkTheme = 0;
		else 
			darkTheme = 1;
	}
	else
	{
		setConfig("/3ds/data/3dstool/darkTheme.bin", false);
		darkTheme = false;
	}
	
}

void termServices(void)
{
	osSetSpeedupEnable(0);
	
	screen_unload_texture(TEXTURE_TOGGLE_OFF);
	screen_unload_texture(TEXTURE_TOGGLE_ON);
	
	screen_exit();
	romfsExit();
	gfxExit();
	
	closeArchive(ARCHIVE_SDMC);
	sdmcExit();
	fsExit();
	cfgiExit();
	amCloseServiceHandle();
}

void mainMenu(void);

void backupMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 5;
	
	Result res = 0;
	
	char func[20];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		screen_draw_rect(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Backup");
		
		screen_draw_rect(0, selector_image_y, 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
		screen_draw_string(10, 65, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup LocalFriendCodeSeed");
		screen_draw_string(10, 125, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup SecureInfo");
		screen_draw_string(10, 155, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup moveable.sed");
		screen_draw_string(10, 185, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup HWCAL");
		
		hidScanInput();

		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		screen_draw_rect(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
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
					if (fileExistsNand("/rw/sys/LocalFriendCodeSeed_B"))
						res = copy_file("/rw/sys/LocalFriendCodeSeed_B", "/3ds/data/3dstool/backups/nand/rw/sys/LocalFriendCodeSeed_B");
					else if (fileExistsNand("/rw/sys/LocalFriendCodeSeed_A"))
						res = copy_file("/rw/sys/LocalFriendCodeSeed_A", "/3ds/data/3dstool/backups/nand/rw/sys/LocalFriendCodeSeed_A");
					snprintf(func, 20, "LocalFriendCodeSeed");
					selection = 1;
					isSelected = true;
					break;
				case 3:
					if (fileExistsNand("/rw/sys/SecureInfo_C"))
						res = copy_file("/rw/sys/SecureInfo_C", "/3ds/data/3dstool/backups/nand/rw/sys/SecureInfo_C");
					if (fileExistsNand("/rw/sys/SecureInfo_A"))
						res = copy_file("/rw/sys/SecureInfo_A", "/3ds/data/3dstool/backups/nand/rw/sys/SecureInfo_A");
					else if (fileExistsNand("/rw/sys/SecureInfo_B"))
						res = copy_file("/rw/sys/SecureInfo_B", "/3ds/data/3dstool/backups/nand/rw/sys/SecureInfo_B");
					snprintf(func, 11, "SecureInfo");
					selection = 1;
					isSelected = true;
					break;
				case 4:
					res = copy_file("/private/movable.sed", "/3ds/data/3dstool/backups/nand/private/movable.sed");
					snprintf(func, 12, "movable.sed");
					selection = 1;
					isSelected = true;
					break;
				case 5:
					res = copy_file("/ro/sys/HWCAL0.dat", "/3ds/data/3dstool/backups/nand/ro/sys/HWCAL0.dat");
					res = copy_file("/ro/sys/HWCAL1.dat", "/3ds/data/3dstool/backups/nand/ro/sys/HWCAL1.dat");
					snprintf(func, 6, "HWCAL");
					selection = 1;
					isSelected = true;
					break;
			}
		}
		
		else if (kDown & KEY_B)
			mainMenu();
		
		if (R_FAILED(res))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s backed-up successfully.", func);
		
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
		screen_draw_rect(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Restore");
		
		screen_draw_rect(0, selector_image_y, 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
		screen_draw_string(10, 65, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore LocalFriendCodeSeed");
		screen_draw_string(10, 125, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore SecureInfo");
		
		hidScanInput();

		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		
		screen_draw_rect(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
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
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s restored successfully.", func);
		
		screen_end_frame();
	}
}

void advancedWipe(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 6;
	
	Result res = 0;
	
	char func[27];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		screen_draw_rect(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Advanced Wipe");
		
		screen_draw_rect(0, selector_image_y, 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
		screen_draw_string(10, 65, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe all temporary and expired titles");
		screen_draw_string(10, 125, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe all TWL titles");
		screen_draw_string(10, 155, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe config");
		screen_draw_string(10, 185, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe parental controls");
		screen_draw_string(10, 215, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe all data");
		
		hidScanInput();

		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		
		screen_draw_rect(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
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
					res = AM_DeleteAllExpiredTitles(MEDIATYPE_SD);
					snprintf(func, 27, "temporary & expired titles");
					selection = 1;
					isSelected = true;
					break;
				case 3:
					res = AM_DeleteAllTwlTitles();
					snprintf(func, 11, "TWL titles");
					selection = 1;
					isSelected = true;
					break;
				case 4:
					res = CFGI_FormatConfig();
					snprintf(func, 7, "config");
					selection = 1;
					isSelected = true;
					break;
				case 5:
					res = CFGI_ClearParentalControls();
					snprintf(func, 18, "parental controls");
					selection = 1;
					isSelected = true;
					break;
				case 6:
					snprintf(func, 5, "NNID");
					selection = 1;
					isSelected = true;
					break;
			}
		}
		
		else if (kDown & KEY_B)
			mainMenu();
		
		if (R_FAILED(res))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe %s failed with err 0x%08x.", func, (unsigned int)res);
		
		screen_end_frame();
	}
}

void miscMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 4;
	
	Result res = 0;
	
	char func[20];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		screen_draw_rect(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Miscellaneous");
		
		screen_draw_rect(0, selector_image_y, 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
		screen_draw_string(10, 65, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back");
		screen_draw_string(10, 95, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Verify LocalFriendCodeSeed sig");
		screen_draw_string(10, 125, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Verify SecureInfo sig");
		screen_draw_string(10, 155, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Dark theme");
		
		darkTheme? screen_draw_texture(TEXTURE_TOGGLE_ON, 350, 145) : screen_draw_texture(TEXTURE_TOGGLE_OFF, 350, 145);
		
		hidScanInput();

		u32 kDown = hidKeysDown();

		screen_select(GFX_BOTTOM);
		
		screen_draw_rect(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
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
				case 4:
					if (darkTheme == false)
					{
						setConfig("/3ds/data/3dstool/darkTheme.bin", true);
						darkTheme = true;
					}
					else
					{
						setConfig("/3ds/data/3dstool/darkTheme.bin", false);
						darkTheme = false;
					}
						
					break;
			}
		}
		
		else if (kDown & KEY_B)
			mainMenu();
		
		if (R_FAILED(res))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Verify %s failed with err 0x%08x.", func, (unsigned int)res);
		else if ((R_SUCCEEDED(res)) && (isSelected))
			screen_draw_stringf(10, 220, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Verified %s successfully.", func);
		
		screen_end_frame();
	}
}

void mainMenu(void)
{
	int selection = 1;
	int selector_y = 25; 
	int selector_image_y = 0;
	
	int max_items = 5;
	
	screen_clear(GFX_TOP, CLEAR_COLOR);
	screen_clear(GFX_BOTTOM, CLEAR_COLOR);
	
	while (aptMainLoop())
	{
		screen_begin_frame();
		screen_select(GFX_TOP);
		
		screen_draw_rect(0, 0, 400, 15, RGBA8(19, 23, 26, 255));
		screen_draw_rect(0, 15, 400, 40, RGBA8(39, 50, 56, 255));
		screen_draw_rect(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		selector_image_y = selector_y + (selector_yDistance * selection);
		
		digitalTime();
		
		screen_draw_string(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "3DS Recovery Tool");
		
		screen_draw_rect(0, selector_image_y, 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
		screen_draw_string(10, 65, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back-up");
		screen_draw_string(10, 95, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore");
		screen_draw_string(10, 125, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Advanced wipe");
		screen_draw_string(10, 155, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Misc");
		screen_draw_string(10, 185, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Exit");
		
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			longjmp(exitJmp, 1);

		screen_select(GFX_BOTTOM);
		
		screen_draw_rect(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		screen_draw_stringf(2, 225, 0.41f, 0.41f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "3DS Recovery Tool v%i.%i0 - %d%02d%02d", VERSION_MAJOR, VERSION_MINOR, YEAR, MONTH, DAY);
		
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