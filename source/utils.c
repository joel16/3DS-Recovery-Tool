#include <stdlib.h>
#include <stdio.h>

#include "fs.h"
#include "utils.h"

bool dark_theme = false;

Result Utils_SaveConfig(bool dark_theme) {
	Result ret = 0;
	
	char *buf = malloc(32);
	int length = snprintf(buf, 32, "dark_theme = %d\n", dark_theme);
	
	if (R_FAILED(ret = FS_Write(sdmc_archive, "/3ds/3DSRecoveryTool/config.cfg", buf, length))) {
		free(buf);
		return ret;
	}
	
	free(buf);
	return 0;
}	

Result Utils_LoadConfig(void) {
	Handle handle;
	Result ret = 0;
	
	if (!FS_FileExists(sdmc_archive, "/3ds/3DSRecoveryTool/config.cfg")) {
		dark_theme = false;
		return Utils_SaveConfig(dark_theme);
	}

	if (R_FAILED(ret = FSUSER_OpenFile(&handle, sdmc_archive, fsMakePath(PATH_ASCII, "/3ds/3DSRecoveryTool/config.cfg"), FS_OPEN_READ, 0)))
		return ret;
	
	u64 size = 0;
	if (R_FAILED(ret = FSFILE_GetSize(handle, &size)))
		return ret;
	
	char *buf = malloc(size + 1);
	if (R_FAILED(ret = FSFILE_Read(handle, NULL, 0, (u32 *)buf, size))) {
		free(buf);
		return ret;
	}
	
	buf[size] = '\0';
	sscanf(buf, "dark_theme = %d\n", (int *)&dark_theme);
	free(buf);
	
	if (R_FAILED(ret = FSFILE_Close(handle)))
		return ret;

	return 0;
}

bool Utils_IsN3DS(void) {
	bool isNew3DS = false;
	
	if (R_SUCCEEDED(APT_CheckNew3DS(&isNew3DS)))
		return isNew3DS;
	
	return false;
}

u16 touchGetX(void) {
	touchPosition pos;	
	hidTouchRead(&pos);
	return pos.px;
}

u16 touchGetY(void) {
	touchPosition pos;	
	hidTouchRead(&pos);
	return pos.py;
}

void Utils_SetMax(int *set, int value, int max) {
	if (*set > max)
		*set = value;
}

void Utils_SetMin(int *set, int value, int min) {
	if (*set < min)
		*set = value;
}
