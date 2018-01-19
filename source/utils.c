#include <stdlib.h>
#include <stdio.h>

#include "fs.h"
#include "utils.h"

const char * configFile =
	"dark_theme = %d\n";
	
Result Utils_SaveConfig(bool dark_theme)
{
	Result ret = 0;
	
	char * buf = (char *)malloc(1024);
	snprintf(buf, 1024, configFile, dark_theme);
	
	if (R_FAILED(ret = FS_WriteFile(sdmcArchive, ARCHIVE_SDMC, "/3ds/3ds_rec_tool/config.cfg", buf)))
	{
		free(buf);
		return ret;
	}
	
	free(buf);
	return 0;
}	

Result Utils_LoadConfig(void)
{
	Handle handle;
	Result ret = 0;
	
	if (!FS_FileExists(sdmcArchive, "/3ds/3ds_rec_tool/config.cfg"))
	{
		darkTheme = false;
		return Utils_SaveConfig(darkTheme);
	}
	
	if (R_FAILED(ret = FSUSER_OpenFileDirectly(&handle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, "/3ds/3ds_rec_tool/config.cfg"), FS_OPEN_READ, 0)))
		return ret;
	
	u64 size64 = 0;
	u32 size = 0;
	
	if (R_FAILED(ret = FSFILE_GetSize(handle, &size64)))
		return ret;
		
	size = (u32)size64;
	
	char * buf = (char *)malloc(size + 1);
	u32 bytesread = 0;
	
	if (R_FAILED(ret = FSFILE_Read(handle, &bytesread, 0, (u32 *)buf, size)))
	{
		free(buf);
		return ret;
	}
	
	buf[size] = '\0';
	
	sscanf(buf, configFile, &darkTheme);

	free(buf);
	
	if (R_FAILED(ret = FSFILE_Close(handle)))
		return ret;

	return 0;
}

bool Utils_IsN3DS(void)
{
	bool isNew3DS = false;
	
	if (R_SUCCEEDED(APT_CheckNew3DS(&isNew3DS)))
		return isNew3DS;
	
	return false;
}

u16 touchGetX(void)
{
	touchPosition pos;	
	hidTouchRead(&pos);
	return pos.px;
}

u16 touchGetY(void)
{
	touchPosition pos;	
	hidTouchRead(&pos);
	return pos.py;
}