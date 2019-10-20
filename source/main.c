#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include <3ds.h>

#include "c2d_helper.h"
#include "dialog.h"
#include "fs.h"
#include "status_bar.h"
#include "textures.h"
#include "utils.h"

#define DISTANCE_Y    30
#define LIST_PER_PAGE 6

static jmp_buf exitJmp;
static int selection = 0;

static bool Utils_IsN3DS(void) {
	bool isNew3DS = false;

	if (R_SUCCEEDED(APT_CheckNew3DS(&isNew3DS)))
		return isNew3DS;

	return false;
}

static void Init_Services(void) {
	amInit();
	cfguInit();
	mcuHwcInit();
	FS_OpenArchive(&sdmc_archive, ARCHIVE_SDMC);
	FS_OpenArchive(&nand_archive, ARCHIVE_NAND_CTR_FS);
	
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	if (Utils_IsN3DS())
		osSetSpeedupEnable(true);

	c2d_static_buf = C2D_TextBufNew(4096);
	c2d_dynamic_buf = C2D_TextBufNew(4096);
	c2d_size_buf = C2D_TextBufNew(4096);
	font = C2D_FontLoad("romfs:/res/drawable/Roboto-Regular.bcfnt");

	RENDER_TOP = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	RENDER_BOTTOM = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	Textures_Load();
	
	FS_RecursiveMakeDir(sdmc_archive, "/3ds/3DSRecoveryTool/dumps");
	FS_RecursiveMakeDir(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/ro/sys");
	FS_RecursiveMakeDir(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys");
	FS_RecursiveMakeDir(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/private");

	Utils_LoadConfig();
}

static void Term_Services(void) {
	Textures_Free();

	C2D_FontFree(font);
	C2D_TextBufDelete(c2d_size_buf);
	C2D_TextBufDelete(c2d_dynamic_buf);
	C2D_TextBufDelete(c2d_static_buf);

	if (Utils_IsN3DS())
		osSetSpeedupEnable(0);
	
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	
	FS_CloseArchive(nand_archive);
	FS_CloseArchive(sdmc_archive);
	mcuHwcExit();
	cfguExit();
	amExit();
}

static void Menu_DrawUI(int *selection, int max_items, const char *item_list[], const char *title, bool main) {
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(RENDER_TOP, C2D_Color32(19, 23, 26, 255));
	C2D_TargetClear(RENDER_BOTTOM, C2D_Color32(39, 50, 56, 255));
	C2D_SceneBegin(RENDER_TOP);

	Draw_Rect(0, 0, 400, 18, C2D_Color32(19, 23, 26, 255));
	Draw_Rect(0, 18, 400, 38, C2D_Color32(39, 50, 56, 255));
	Draw_Rect(0, 55, 400, 185, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);

	StatusBar_DisplayBar();
	
	if (main)
		Draw_Text(10, 27, 0.5f, C2D_Color32(240, 242, 242, 255), title);
	else {
		Draw_Image(icon_back, 5, 22);
		Draw_Text(30 + 10, 27, 0.5f, C2D_Color32(240, 242, 242, 255), title);
	}
	
	int printed = 0;
	for (int i = 0; i < max_items + 1; i++) {
		if (printed == LIST_PER_PAGE)
			break;
			
		if (*selection < LIST_PER_PAGE || i > (*selection - LIST_PER_PAGE)) {
			if (i == *selection)
				Draw_Rect(0, 55 + (DISTANCE_Y * printed), 400, 30, dark_theme? SELECTOR_COLOUR_DARK : SELECTOR_COLOUR_LIGHT);
			
			Draw_Text(10, 62 + (DISTANCE_Y * printed), 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, item_list[i]);
			printed++;
		}
	}
}

static void Menu_HandleControls(u32 *kDown, int max_items) {
	hidScanInput();
	*kDown = hidKeysDown();
	
	if (*kDown & KEY_START)
		longjmp(exitJmp, 1);

	if (*kDown & KEY_DDOWN)
		selection++;
	else if (*kDown & KEY_DUP)
		selection--;

	Utils_SetMax(&selection, 0, max_items);
	Utils_SetMin(&selection, max_items, 0);
}

static void Menu_Backup(void) {
	Result ret = 0;
	selection = 0;
	int max_items = 4;
	bool is_selected = false;
	char func[50];
	const char *item_list[] = {
		"Back",
		"Backup NAND LocalFriendCodeSeed",
		"Backup NAND SecureInfo",
		"Backup moveable.sed",
		"Backup HWCAL"
	};

	while (aptMainLoop()) {
		Menu_DrawUI(&selection, max_items, item_list, "Backup", false);
		
		C2D_SceneBegin(RENDER_BOTTOM);
		Draw_Rect(0, 0, 320, 240, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		if (R_FAILED(ret))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Backup %s failed with err 0x%08x.", func, (unsigned int)ret);
		else if ((R_SUCCEEDED(ret)) && (is_selected))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s backed-up successfully.", func);
		
		Draw_EndFrame();

		u32 kDown = 0;
		Menu_HandleControls(&kDown, max_items);

		if (kDown & KEY_A) {
			if (selection == 0)
				break;
			
			switch(selection) {
				case 1:
					if (FS_FileExists(nand_archive, "/rw/sys/LocalFriendCodeSeed_B"))
						ret = FS_CopyFile(nand_archive, sdmc_archive, "/rw/sys/LocalFriendCodeSeed_B", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_B");
					else if (FS_FileExists(nand_archive, "/rw/sys/LocalFriendCodeSeed_A"))
						ret = FS_CopyFile(nand_archive, sdmc_archive, "/rw/sys/LocalFriendCodeSeed_A", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_A");
					
					snprintf(func, 20, "LocalFriendCodeSeed");
					break;
				
				case 2:
					if (FS_FileExists(nand_archive, "/rw/sys/SecureInfo_C"))
						ret = FS_CopyFile(nand_archive, sdmc_archive, "/rw/sys/SecureInfo_C", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_C");
					if (FS_FileExists(nand_archive, "/rw/sys/SecureInfo_A"))
						ret = FS_CopyFile(nand_archive, sdmc_archive, "/rw/sys/SecureInfo_A", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_A");
					else if (FS_FileExists(nand_archive, "/rw/sys/SecureInfo_B"))
						ret = FS_CopyFile(nand_archive, sdmc_archive, "/rw/sys/SecureInfo_B", "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_B");
					
					snprintf(func, 11, "SecureInfo");
					break;
				
				case 3:
					ret = FS_CopyFile(nand_archive, sdmc_archive, "/private/movable.sed", "/3ds/3DSRecoveryTool/backups/nand/private/movable.sed");
					snprintf(func, 12, "movable.sed");
					break;
				
				case 4:
					ret = FS_CopyFile(nand_archive, sdmc_archive, "/ro/sys/HWCAL0.dat", "/3ds/3DSRecoveryTool/backups/nand/ro/sys/HWCAL0.dat");
					ret = FS_CopyFile(nand_archive, sdmc_archive, "/ro/sys/HWCAL1.dat", "/3ds/3DSRecoveryTool/backups/nand/ro/sys/HWCAL1.dat");
					snprintf(func, 6, "HWCAL");
					break;
			}

			is_selected = true;
			selection = 0;
		}
		else if (kDown & KEY_B)
			break;
	}
}

static void Menu_Restore(void) {
	Result ret = 0;
	selection = 0;
	int max_items = 4;
	bool is_selected = false;
	char func[50];
	const char *item_list[] = {
		"Back",
		"Restore original LocalFriendCodeSeed",
		"Restore original SecureInfo",
		"Restore backup LocalFriendCodeSeed",
		"Restore backup SecureInfo"
	};

	while (aptMainLoop()) {
		Menu_DrawUI(&selection, max_items, item_list, "Restore", false);
		
		C2D_SceneBegin(RENDER_BOTTOM);
		Draw_Rect(0, 0, 320, 240, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		if (R_FAILED(ret))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Restore %s failed with err 0x%08x.", func, (unsigned int)ret);
		else if ((R_SUCCEEDED(ret)) && (is_selected))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s restored successful.", func);
		
		Draw_EndFrame();

		u32 kDown = 0;
		Menu_HandleControls(&kDown, max_items);

		if (kDown & KEY_A) {
			if (selection == 0)
				break;
			
			switch(selection) {
				case 1:
					ret = CFGI_RestoreLocalFriendCodeSeed();
					snprintf(func, 20, "LocalFriendCodeSeed");
					break;
				
				case 2:
					ret = CFGI_RestoreSecureInfo();
					snprintf(func, 11, "SecureInfo");
				
				case 3:
					if (FS_FileExists(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_B"))
						ret = FS_CopyFile(sdmc_archive, nand_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_B", "/rw/sys/LocalFriendCodeSeed_B");
					else if (FS_FileExists(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_A"))
						ret = FS_CopyFile(sdmc_archive, nand_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/LocalFriendCodeSeed_A", "/rw/sys/LocalFriendCodeSeed_A");
					
					snprintf(func, 20, "LocalFriendCodeSeed");
					break;
				
				case 4:
					if (FS_FileExists(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_C"))
						ret = FS_CopyFile(sdmc_archive, nand_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_C", "/rw/sys/SecureInfo_C");
					if (FS_FileExists(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_A"))
						ret = FS_CopyFile(sdmc_archive, nand_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_A", "/rw/sys/SecureInfo_A");
					else if (FS_FileExists(sdmc_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_B"))
						ret = FS_CopyFile(sdmc_archive, nand_archive, "/3ds/3DSRecoveryTool/backups/nand/rw/sys/SecureInfo_B", "/rw/sys/SecureInfo_B");
					
					snprintf(func, 11, "SecureInfo");
					break;
			}

			is_selected = true;
			selection = 0;
		}
		else if (kDown & KEY_B)
			break;
	}
}

static void Menu_Advanced_Wipe(void) {
	Result ret = 0;
	selection = 0;
	int max_items = 9;
	bool is_selected = false;
	char func[50];
	const char *item_list[] = {
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

	while (aptMainLoop()) {
		Menu_DrawUI(&selection, max_items, item_list, "Advanced Wipe", false);

		C2D_SceneBegin(RENDER_BOTTOM);
		Draw_Rect(0, 0, 320, 240, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		if (R_FAILED(ret))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wipe %s failed with err 0x%08x.", func, (unsigned int)ret);
		else if ((R_SUCCEEDED(ret)) && (is_selected))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "Wiped %s successful.", func);
		
		Draw_EndFrame();

		u32 kDown = 0;
		Menu_HandleControls(&kDown, max_items);

		if (kDown & KEY_A) {
			if (selection == 0)
				break;
			
			switch(selection) {
				case 1:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all expired titles.", "Do you wish to continue?"))) {
						ret = AM_DeleteAllTemporaryTitles();
						ret = AM_DeleteAllExpiredTitles(MEDIATYPE_SD);
						snprintf(func, 27, "temporary & expired titles");
					}
					break;
				
				case 2:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all TWL titles.", "Do you wish to continue?"))) {
						ret = AM_DeleteAllTwlTitles();
						snprintf(func, 11, "TWL titles");
					}
					break;
				
				case 3:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all pending tiles.", "Do you wish to continue?"))) {
						ret = AM_DeleteAllPendingTitles(MEDIATYPE_NAND);
						ret = AM_DeleteAllPendingTitles(MEDIATYPE_SD);
						snprintf(func, 14, "pending tiles");
					}
					break;
				
				case 4:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all demo launch infos.", "Do you wish to continue?"))) {
						ret = AM_DeleteAllDemoLaunchInfos();
						snprintf(func, 18, "demo launch infos");
					}
					break;
				
				case 5:
					if (R_SUCCEEDED(Dialog_Draw("You will lose all data in Settings.", "Do you wish to continue?"))) {
						ret = CFGI_FormatConfig();
						snprintf(func, 7, "config");
					}
					break;
				
				case 6:
					if (R_SUCCEEDED(Dialog_Draw("This will disable parental controls.", "Do you wish to continue?"))) {
						ret = CFGI_ClearParentalControls();
						snprintf(func, 18, "parental controls");
					}
					break;
				
				case 7:
					if (R_SUCCEEDED(Dialog_Draw("You will lose ALL data.", "Do you wish to continue?"))) {
						ret = FSUSER_DeleteAllExtSaveDataOnNand();
						ret = FSUSER_InitializeCtrFileSystem();
						snprintf(func, 9, "all data");
					}
					break;
				
				case 8:
					if (R_SUCCEEDED(Dialog_Draw("You will lose ALL data in your SD.", "Do you wish to continue?"))) {
						ret = FSUSER_DeleteSdmcRoot();
						snprintf(func, 5, "SDMC");
					}
					break;
				
				case 9:
					if (R_SUCCEEDED(Dialog_Draw("You will lose ALL ext savedata in nand.", "Do you wish to continue?"))) {
						ret = FSUSER_DeleteAllExtSaveDataOnNand();
						snprintf(func, 18, "NAND ext savedata");
					}
			}

			is_selected = true;
			selection = 0;
		}
		else if (kDown & KEY_B)
			break;
	}
}

static void Menu_Misc(void) {
	Result ret = 0;
	selection = 0;
	int max_items = 5;
	bool is_selected = false;
	char func[50];
	const char *item_list[] = {
		"Back",
		"Dump original LocalFriendCodeSeed data",
		"Dump original SecureInfo data",
		"Verify LocalFriendCodeSeed sig",
		"Verify SecureInfo sig",
		"Dark theme"		
	};

	u8 lfcs_data[0x110], sig_data[0x100], secureinfo_data[0x11], secureinfo_sig_data[0x111];

	while (aptMainLoop()) {
		Menu_DrawUI(&selection, max_items, item_list, "Miscellaneous", false);
		dark_theme? Draw_Image(icon_toggle_on, 350, 205) : Draw_Image(icon_toggle_off, 350, 205);
		
		C2D_SceneBegin(RENDER_BOTTOM);
		Draw_Rect(0, 0, 320, 240, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		
		if (R_FAILED(ret))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s failed with err 0x%08x.", func, (unsigned int)ret);
		else if ((R_SUCCEEDED(ret)) && (is_selected))
			Draw_Textf(10, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "%s successful.", func);
		
		Draw_EndFrame();
		
		u32 kDown = 0;
		Menu_HandleControls(&kDown, max_items);

		if (kDown & KEY_A) {
			if (selection == 0)
				break;
			
			switch(selection) {
				case 1:
					if (R_SUCCEEDED(ret = CFGI_GetLocalFriendCodeSeedData(lfcs_data))) {
						if (FS_FileExists(nand_archive, "/rw/sys/LocalFriendCodeSeed_B"))
							ret = FS_Write(sdmc_archive, "/3ds/3DSRecoveryTool/dumps/LocalFriendCodeSeed_B", lfcs_data, 0x110);
						else if (FS_FileExists(nand_archive, "/rw/sys/LocalFriendCodeSeed_A"))
							ret = FS_Write(sdmc_archive, "/3ds/3DSRecoveryTool/dumps/LocalFriendCodeSeed_A", lfcs_data, 0x110);
					}

					snprintf(func, 25, "LocalFriendCodeSeed dump");
					selection = 0;
					is_selected = true;
					break;
				
				case 2:
					if (R_SUCCEEDED(ret = CFGI_GetSecureInfoSignature(sig_data)) && R_SUCCEEDED(ret = CFGI_GetSecureInfoData(secureinfo_data))) {
						memcpy(&secureinfo_sig_data[0x0], sig_data, 0x100);
						memcpy(&secureinfo_sig_data[0x100], secureinfo_data, 0x11);

						if (FS_FileExists(nand_archive, "/rw/sys/SecureInfo_C"))
							ret = FS_Write(sdmc_archive, "/3ds/3DSRecoveryTool/dumps/SecureInfo_C", secureinfo_sig_data, 0x111);
						else if (FS_FileExists(nand_archive, "/rw/sys/SecureInfo_A"))
							ret = FS_Write(sdmc_archive, "/3ds/3DSRecoveryTool/dumps/SecureInfo_A", secureinfo_sig_data, 0x111);
						else if (FS_FileExists(nand_archive, "/rw/sys/SecureInfo_B"))
							ret = FS_Write(sdmc_archive, "/3ds/3DSRecoveryTool/dumps/SecureInfo_B", secureinfo_sig_data, 0x111);
					}
					
					snprintf(func, 25, "SecureInfo dump");
					selection = 0;
					is_selected = true;
					break;
				
				case 3:
					ret = CFGI_VerifySigLocalFriendCodeSeed();
					snprintf(func, 33, "LocalFriendCodeSeed verification");
					selection = 0;
					is_selected = true;
					break;
				
				case 4:
					ret = CFGI_VerifySigSecureInfo();
					snprintf(func, 24, "SecureInfo verification");
					selection = 0;
					is_selected = true;
					break;
				
				case 5:
					if (dark_theme == false)
						dark_theme = true;
					else
						dark_theme = false;
					Utils_SaveConfig(dark_theme);
					break;
			}
		}
		else if (kDown & KEY_B)
			break;
	}
}

static void Menu_Main(void) {
	selection = 0;
	int max_items = 4;
	const char *item_list[] = {
		"Back-up",
		"Restore",
		"Advanced wipe",
		"Misc",
		"Exit"
	};

	while (aptMainLoop()) {
		Menu_DrawUI(&selection, max_items, item_list, "3DS Recovery Tool", true);
		
		C2D_SceneBegin(RENDER_BOTTOM);
		Draw_Rect(0, 0, 320, 240, dark_theme? BG_COLOUR_DARK : BG_COLOUR_LIGHT);
		Draw_Textf(5, 220, 0.45f, dark_theme? TEXT_COLOUR_DARK : TEXT_COLOUR_LIGHT, "3DS Recovery Tool v%d.%d%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
		
		Draw_EndFrame();

		u32 kDown = 0;
		Menu_HandleControls(&kDown, max_items);

		if (kDown & KEY_A) {
			switch(selection) {
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

int main(int argc, char **argv) {
	Init_Services();
	if (setjmp(exitJmp)) {
		Term_Services();
		return 0;
	}
	
	Menu_Main();
	Term_Services();
	
	return 0;
}
