#include <3ds.h>

#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include "colours.h"
#include "dialog.h"
#include "fs.h"
#include "pp2d.h"
#include "status_bar.h"
#include "textures.h"
#include "utils.h"

#define DISTANCE_Y    30
#define LIST_PER_PAGE 6

jmp_buf exitJmp;

void Init_Services(void)
{
	amInit();
	cfguInit();
	FS_OpenArchive(&sdmcArchive, ARCHIVE_SDMC);
	FS_OpenArchive(&nandArchive, ARCHIVE_NAND_CTR_FS);
	
	romfsInit();
	pp2d_init();
	
	pp2d_load_texture_png(TEXTURE_TOGGLE_ON, "romfs:/res/drawable/toggleOn.png");
	pp2d_load_texture_png(TEXTURE_TOGGLE_OFF, "romfs:/res/drawable/toggleOff.png");

	pp2d_load_texture_png(TEXTURE_BATTERY_0, "romfs:/res/drawable/battery/0.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_15, "romfs:/res/drawable/battery/15.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_28, "romfs:/res/drawable/battery/28.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_43, "romfs:/res/drawable/battery/43.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_57, "romfs:/res/drawable/battery/57.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_71, "romfs:/res/drawable/battery/71.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_85, "romfs:/res/drawable/battery/85.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_100, "romfs:/res/drawable/battery/100.png");
	pp2d_load_texture_png(TEXTURE_BATTERY_CHARGE, "romfs:/res/drawable/battery/charge.png");

	pp2d_load_texture_png(TEXTURE_WIFI_NULL, "romfs:/res/drawable/wifi/stat_sys_wifi_signal_null.png");
	pp2d_load_texture_png(TEXTURE_WIFI_0, "romfs:/res/drawable/wifi/stat_sys_wifi_signal_0.png");
	pp2d_load_texture_png(TEXTURE_WIFI_1, "romfs:/res/drawable/wifi/stat_sys_wifi_signal_1.png");
	pp2d_load_texture_png(TEXTURE_WIFI_2, "romfs:/res/drawable/wifi/stat_sys_wifi_signal_2.png");
	pp2d_load_texture_png(TEXTURE_WIFI_3, "romfs:/res/drawable/wifi/stat_sys_wifi_signal_3.png");
	
	if (Utils_IsN3DS())
		osSetSpeedupEnable(true);
	
	FS_RecursiveMakeDir(sdmcArchive, "/3ds/3DSRecoveryTool/dumps");
	FS_RecursiveMakeDir(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/ro/sys");
	FS_RecursiveMakeDir(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys");
	FS_RecursiveMakeDir(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/private");

	Utils_LoadConfig();
}

void Term_Services(void)
{
	pp2d_free_texture(TEXTURE_WIFI_3);
	pp2d_free_texture(TEXTURE_WIFI_2);
	pp2d_free_texture(TEXTURE_WIFI_1);
	pp2d_free_texture(TEXTURE_WIFI_0);
	pp2d_free_texture(TEXTURE_WIFI_NULL);

	pp2d_free_texture(TEXTURE_BATTERY_CHARGE);
	pp2d_free_texture(TEXTURE_BATTERY_100);
	pp2d_free_texture(TEXTURE_BATTERY_85);
	pp2d_free_texture(TEXTURE_BATTERY_71);
	pp2d_free_texture(TEXTURE_BATTERY_57);
	pp2d_free_texture(TEXTURE_BATTERY_43);
	pp2d_free_texture(TEXTURE_BATTERY_28);
	pp2d_free_texture(TEXTURE_BATTERY_15);
	pp2d_free_texture(TEXTURE_BATTERY_0);
	
	pp2d_free_texture(TEXTURE_TOGGLE_OFF);
	pp2d_free_texture(TEXTURE_TOGGLE_ON);

	if (Utils_IsN3DS())
		osSetSpeedupEnable(0);
	
	pp2d_exit();
	romfsExit();
	
	FS_CloseArchive(nandArchive);
	FS_CloseArchive(sdmcArchive);
	cfguExit();
	amExit();
}

void Menu_Main(void);

void Menu_Backup(void)
{
	int selection = 0, max_items = 4;
	
	Result res = 0;
	
	char func[20];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 400, 16, RGBA8(19, 23, 26, 255));
			pp2d_draw_rectangle(0, 16, 400, 40, RGBA8(39, 50, 56, 255));
			pp2d_draw_rectangle(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			StatusBar_DisplayBar();
		
			pp2d_draw_text(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Backup");
		
			pp2d_draw_rectangle(0, 55 + (DISTANCE_Y * selection), 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
			pp2d_draw_text(10, 65, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back");
			pp2d_draw_text(10, 95, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup NAND LocalFriendCodeSeed");
			pp2d_draw_text(10, 125, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup NAND SecureInfo");
			pp2d_draw_text(10, 155, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup moveable.sed");
			pp2d_draw_text(10, 185, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup HWCAL");
		
		pp2d_end_draw();

		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
			
			pp2d_draw_rectangle(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			if (R_FAILED(res))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup %s failed with err 0x%08x.", func, (unsigned int)res);
			else if ((R_SUCCEEDED(res)) && (isSelected))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s backed-up successfully.", func);
		
		pp2d_end_draw();

		hidScanInput();
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_DDOWN)
			selection++;
		else if (kDown & KEY_DUP)
			selection--;
		
		if (selection > max_items) 
			selection = 0;
		if (selection < 0) 
			selection = max_items;
		
		if (kDown & KEY_A)
		{
			switch(selection)
			{
				case 0:
					Menu_Main();
					break;
				case 1:
					if (FS_FileExists(nandArchive, "/rw/sys/LocalFriendCodeSeed_B"))
						res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/rw/sys/LocalFriendCodeSeed_B", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_B");
					else if (FS_FileExists(nandArchive, "/rw/sys/LocalFriendCodeSeed_A"))
						res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/rw/sys/LocalFriendCodeSeed_A", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_A");
					
					snprintf(func, 20, "LocalFriendCodeSeed");
					break;
				case 2:
					if (FS_FileExists(nandArchive, "/rw/sys/SecureInfo_C"))
						res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/rw/sys/SecureInfo_C", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_C");
					if (FS_FileExists(nandArchive, "/rw/sys/SecureInfo_A"))
						res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/rw/sys/SecureInfo_A", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_A");
					else if (FS_FileExists(nandArchive, "/rw/sys/SecureInfo_B"))
						res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/rw/sys/SecureInfo_B", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_B");
					
					snprintf(func, 11, "SecureInfo");
					break;
				case 3:
					res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/private/movable.sed", "/3ds/3DSRecoveryTool/backups/nand/private/movable.sed");
					snprintf(func, 12, "movable.sed");
					break;
				case 4:
					res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/ro/sys/HWCAL0.dat", "/3ds/3DSRecoveryTool/backups/nand/ro/sys/HWCAL0.dat");
					res = FS_Copy_File(nandArchive, sdmcArchive, ARCHIVE_NAND_CTR_FS, ARCHIVE_SDMC, "/ro/sys/HWCAL1.dat", "/3ds/3DSRecoveryTool/backups/nand/ro/sys/HWCAL1.dat");
					snprintf(func, 6, "HWCAL");
					break;
			}

			isSelected = true;
			selection = 0;
		}
		
		else if (kDown & KEY_B)
			Menu_Main();
	}
}

void Menu_Restore(void)
{
	int selection = 0, max_items = 4;
	
	Result res = 0;
	
	char func[20];
	
	bool isSelected = false;
	
	while (aptMainLoop())
	{
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 400, 16, RGBA8(19, 23, 26, 255));
			pp2d_draw_rectangle(0, 16, 400, 40, RGBA8(39, 50, 56, 255));
			pp2d_draw_rectangle(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			StatusBar_DisplayBar();
		
			pp2d_draw_text(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Restore");
		
			pp2d_draw_rectangle(0, 55 + (DISTANCE_Y * selection), 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
			pp2d_draw_text(10, 65, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back");
			pp2d_draw_text(10, 95, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore original LocalFriendCodeSeed");
			pp2d_draw_text(10, 125, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore original SecureInfo");
			pp2d_draw_text(10, 155, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore backup LocalFriendCodeSeed");
			pp2d_draw_text(10, 185, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore backup SecureInfo");

		pp2d_end_draw();

		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			if (R_FAILED(res))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore %s failed with err 0x%08x.", func, (unsigned int)res);
			else if ((R_SUCCEEDED(res)) && (isSelected))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s restored successfully.", func);
		
		pp2d_end_draw();

		hidScanInput();
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_DDOWN)
			selection++;
		else if (kDown & KEY_DUP)
			selection--;
		
		if (selection > max_items) 
			selection = 0;
		if (selection < 0) 
			selection = max_items;
		
		if (kDown & KEY_A)
		{
			switch(selection)
			{
				case 0:
					Menu_Main();
					break;
				case 1:
					res = CFGI_RestoreLocalFriendCodeSeed();
					snprintf(func, 20, "LocalFriendCodeSeed");
					break;
				case 2:
					res = CFGI_RestoreSecureInfo();
					snprintf(func, 11, "SecureInfo");
				case 3:
					if (FS_FileExists(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_B"))
						res = FS_Copy_File(sdmcArchive, nandArchive, ARCHIVE_SDMC, ARCHIVE_NAND_CTR_FS, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_B", "/rw/sys/LocalFriendCodeSeed_B");
					else if (FS_FileExists(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_A"))
						res = FS_Copy_File(sdmcArchive, nandArchive, ARCHIVE_SDMC, ARCHIVE_NAND_CTR_FS, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_A", "/rw/sys/LocalFriendCodeSeed_A");
					
					snprintf(func, 20, "LocalFriendCodeSeed");
					break;
				case 4:
					if (FS_FileExists(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_C"))
						res = FS_Copy_File(sdmcArchive, nandArchive, ARCHIVE_SDMC, ARCHIVE_NAND_CTR_FS, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_C", "/rw/sys/SecureInfo_C");
					if (FS_FileExists(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_A"))
						res = FS_Copy_File(sdmcArchive, nandArchive, ARCHIVE_SDMC, ARCHIVE_NAND_CTR_FS, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_A", "/rw/sys/SecureInfo_A");
					else if (FS_FileExists(sdmcArchive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_B"))
						res = FS_Copy_File(sdmcArchive, nandArchive, ARCHIVE_SDMC, ARCHIVE_NAND_CTR_FS, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_B", "/rw/sys/SecureInfo_B");
					
					snprintf(func, 11, "SecureInfo");
					break;
			}

			isSelected = true;
			selection = 0;
		}
		
		else if (kDown & KEY_B)
			Menu_Main();
	}
}

void Menu_Advanced_Wipe(void)
{
	int selection = 0, max_items = 10;
	
	Result res = 0;
	
	char func[27];
	
	bool isSelected = false;

	const char * wipe_list[] = 
	{
		"Back",
		"Wipe all temporary and expired titles",
		"Wipe all TWL titles",
		"Wipe all pending titles",
		"Wipe all demo launch infos",
		"Wipe config",
		"Wipe parental controls",
		"Wipe all data (NAND)",
		"Format SDMC root",
		"Format NAND ext savedata"
	};
	
	while (aptMainLoop())
	{
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 400, 16, RGBA8(19, 23, 26, 255));
			pp2d_draw_rectangle(0, 16, 400, 40, RGBA8(39, 50, 56, 255));
			pp2d_draw_rectangle(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			StatusBar_DisplayBar();
		
			pp2d_draw_text(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Advanced Wipe");

			int printed = 0; // Print counter

			for (int i = 0; i < max_items + 1; i++)
			{
				if (printed == LIST_PER_PAGE)
					break;

				if (selection < LIST_PER_PAGE || i > (selection - LIST_PER_PAGE))
				{
					if (i == selection)
						pp2d_draw_rectangle(0, 55 + (DISTANCE_Y * printed), 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);

					pp2d_draw_text(10, 65 + (DISTANCE_Y * printed), 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, wipe_list[i]);

					printed++;
				}
			}

		pp2d_end_draw();

		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			if (R_FAILED(res))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe %s failed with err 0x%08x.", func, (unsigned int)res);
			else if ((R_SUCCEEDED(res)) && (isSelected))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wiped %s successfully.", func);
		
		pp2d_end_draw();

		hidScanInput();
		u32 kDown = hidKeysDown();
			
		if (kDown & KEY_DDOWN)
		{
			if (selection < (max_items - 1))
				selection++;
			else 
				selection = 0;
		}
		else if (kDown & KEY_DUP)
		{
			if (selection > 0)
				selection--;
			else 
				selection = (max_items - 1);
		}
		
		if (kDown & KEY_A)
		{
			switch(selection)
			{
				case 0:
					Menu_Main();
					break;
				case 1:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all expired titles.", "Do you wish to continue?")))
					{
						res = AM_DeleteAllTemporaryTitles();
						res = AM_DeleteAllExpiredTitles(MEDIATYPE_SD);
						snprintf(func, 27, "temporary & expired titles");
					}
					break;
				case 2:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all TWL titles.", "Do you wish to continue?")))
					{
						res = AM_DeleteAllTwlTitles();
						snprintf(func, 11, "TWL titles");
					}
					break;
				case 3:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all pending tiles.", "Do you wish to continue?")))
					{
						res = AM_DeleteAllPendingTitles(MEDIATYPE_NAND);
						res = AM_DeleteAllPendingTitles(MEDIATYPE_SD);
						snprintf(func, 14, "pending tiles");
					}
					break;
				case 4:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all demo launch infos.", "Do you wish to continue?")))
					{
						res = AM_DeleteAllDemoLaunchInfos();
						snprintf(func, 18, "demo launch infos");
					}
					break;
				case 5:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all data in Settings.", "Do you wish to continue?")))
					{
						res = CFGI_FormatConfig();
						snprintf(func, 7, "config");
					}
					break;
				case 6:
					if (R_SUCCEEDED(Dialog_Draw("This will disable parental controls.", "Do you wish to continue?")))
					{
						res = CFGI_ClearParentalControls();
						snprintf(func, 18, "parental controls");
					}
					break;
				case 7:
					if (R_SUCCEEDED(Dialog_Draw("You will lose ALL data.", "Do you wish to continue?")))
					{
						res = FSUSER_DeleteAllExtSaveDataOnNand();
						res = FSUSER_InitializeCtrFileSystem();
						snprintf(func, 9, "all data");
					}
					break;
				case 8:
					if (R_SUCCEEDED(Dialog_Draw("You will lose ALL data in your SD.", "Do you wish to continue?")))
					{
						res = FSUSER_DeleteSdmcRoot();
						snprintf(func, 5, "SDMC");
					}
					break;
				case 9:
					if (R_SUCCEEDED(Dialog_Draw("You will lose ALL ext savedata in nand.", "Do you wish to continue?")))
					{
						res = FSUSER_DeleteAllExtSaveDataOnNand();
						snprintf(func, 18, "NAND ext savedata");
					}
			}

			isSelected = true;
			selection = 0;
		}
		
		else if (kDown & KEY_B)
			Menu_Main();
	}
}

void Menu_Misc(void)
{
	int selection = 0, max_items = 5;
	
	Result res = 0;
	
	char func[33];
	
	bool isSelected = false;

	FILE *fp = NULL;
	u8 lfcs_data[0x110], sig_data[0x100], secureinfo_data[0x11];

	CFGI_GetSecureInfoSignature(sig_data);
	CFGI_GetSecureInfoData(secureinfo_data);
	
	while (aptMainLoop())
	{
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 400, 16, RGBA8(19, 23, 26, 255));
			pp2d_draw_rectangle(0, 16, 400, 40, RGBA8(39, 50, 56, 255));
			pp2d_draw_rectangle(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			StatusBar_DisplayBar();
		
			pp2d_draw_text(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "Miscellaneous");
		
			pp2d_draw_rectangle(0, 55 + (DISTANCE_Y * selection), 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
			pp2d_draw_text(10, 65, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back");
			pp2d_draw_text(10, 95, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Dump original LocalFriendCodeSeed data");
			pp2d_draw_text(10, 125, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Dump original SecureInfo data");
			pp2d_draw_text(10, 155, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Verify LocalFriendCodeSeed sig");
			pp2d_draw_text(10, 185, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Verify SecureInfo sig");
			pp2d_draw_text(10, 215, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Dark theme");
		
			darkTheme? pp2d_draw_texture(TEXTURE_TOGGLE_ON, 350, 205) : pp2d_draw_texture(TEXTURE_TOGGLE_OFF, 350, 205);
		
		pp2d_end_draw();

		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);

			if (R_FAILED(res))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s failed with err 0x%08x.", func, (unsigned int)res);
			else if ((R_SUCCEEDED(res)) && (isSelected))
				pp2d_draw_textf(10, 220, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s successful.", func);

		pp2d_end_draw();

		hidScanInput();
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_DDOWN)
			selection++;
		else if (kDown & KEY_DUP)
			selection--;
		
		if (selection > max_items) 
			selection = 0;
		if (selection < 0) 
			selection = max_items;
		
		if (kDown & KEY_A)
		{
			switch(selection)
			{
				case 0:
					Menu_Main();
					break;
				case 1:
					CFGI_GetLocalFriendCodeSeedData(lfcs_data);

					if (FS_FileExists(nandArchive, "/rw/sys/LocalFriendCodeSeed_B"))
						fp = fopen ("/3ds/3DSRecoveryTool/dumps/LocalFriendCodeSeed_B", "wb");
					else if (FS_FileExists(nandArchive, "/rw/sys/LocalFriendCodeSeed_A"))
						fp = fopen ("/3ds/3DSRecoveryTool/dumps/LocalFriendCodeSeed_A", "wb");

					res = fwrite(lfcs_data, 1, 0x110, fp);
					fclose(fp);
					
					snprintf(func, 25, "LocalFriendCodeSeed dump");
					selection = 0;
					isSelected = true;
					break;
				case 2:
					if (FS_FileExists(nandArchive, "/rw/sys/SecureInfo_C"))
						fp = fopen ("/3ds/3DSRecoveryTool/dumps/SecureInfo_C", "wb");
					else if (FS_FileExists(nandArchive, "/rw/sys/SecureInfo_A"))
						fp = fopen ("/3ds/3DSRecoveryTool/dumps/SecureInfo_A", "wb");
					else if (FS_FileExists(nandArchive, "/rw/sys/SecureInfo_B"))
						fp = fopen ("/3ds/3DSRecoveryTool/dumps/SecureInfo_B", "wb");

					res = fwrite(sig_data, 1, 0x100, fp);
					res = fwrite(secureinfo_data, 1, 0x11, fp);
					fclose(fp);
					
					snprintf(func, 25, "SecureInfo dump");
					selection = 0;
					isSelected = true;
					break;
				case 3:
					res = CFGI_VerifySigLocalFriendCodeSeed();
					snprintf(func, 33, "LocalFriendCodeSeed verification");
					selection = 0;
					isSelected = true;
					break;
				case 4:
					res = CFGI_VerifySigSecureInfo();
					snprintf(func, 24, "SecureInfo verification");
					selection = 0;
					isSelected = true;
					break;
				case 5:
					if (darkTheme == false)
						darkTheme = true;
					else
						darkTheme = false;
					Utils_SaveConfig(darkTheme);
					break;
			}
		}
		
		else if (kDown & KEY_B)
			Menu_Main();
	}
}

void Menu_Main(void)
{
	int selection = 0, max_items = 4;
	
	pp2d_set_screen_color(GFX_TOP, 0x000000FF);
	pp2d_set_screen_color(GFX_BOTTOM, 0x000000FF);
	
	while (aptMainLoop())
	{
		pp2d_begin_draw(GFX_TOP, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 400, 16, RGBA8(19, 23, 26, 255));
			pp2d_draw_rectangle(0, 16, 400, 40, RGBA8(39, 50, 56, 255));
			pp2d_draw_rectangle(0, 55, 400, 185, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			StatusBar_DisplayBar();
		
			pp2d_draw_text(10, 27, 0.5f, 0.5f, RGBA8(240, 242, 242, 255), "3DS Recovery Tool");
		
			pp2d_draw_rectangle(0, 55 + (DISTANCE_Y * selection), 400, 30, darkTheme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
		
			pp2d_draw_text(10, 65, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Back-up");
			pp2d_draw_text(10, 95, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore");
			pp2d_draw_text(10, 125, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Advanced wipe");
			pp2d_draw_text(10, 155, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Misc");
			pp2d_draw_text(10, 185, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Exit");
		
		pp2d_end_draw();

		pp2d_begin_draw(GFX_BOTTOM, GFX_LEFT);
		
			pp2d_draw_rectangle(0, 0, 320, 240, darkTheme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
			pp2d_draw_textf(2, 225, 0.45f, 0.45f, darkTheme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "3DS Recovery Tool v%i.%i0", VERSION_MAJOR, VERSION_MINOR);
		
		pp2d_end_draw();

		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			longjmp(exitJmp, 1);
		
		if (kDown & KEY_DDOWN)
			selection++;
		else if (kDown & KEY_DUP)
			selection--;
		
		if (selection > max_items) 
			selection = 0;
		if (selection < 0) 
			selection = max_items;
		
		if (kDown & KEY_A)
		{
			switch(selection)
			{
				case 0:
					Menu_Backup();
					break;
				case 1:
					Menu_Restore();
					break;
				case 2:
					Menu_Advanced_Wipe();
					break;
				case 3:
					Menu_Misc();
					break;
				case 4:
					longjmp(exitJmp, 1);
					break;
			}
		}
	}
}

int main(int argc, char **argv) 
{
	Init_Services();
	
	if(setjmp(exitJmp)) 
	{
		Term_Services();
		return 0;
	}
	
	Menu_Main();
	
	Term_Services();
	
	return 0;
}