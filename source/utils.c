#include <stdlib.h>
#include <stdio.h>

#include "fs.h"
#include "utils.h"

const char * configFile =
	"dark_theme = %d\n";
	
Result saveConfig(bool dark_theme)
{
	Result ret = 0;
	
	char * buf = (char *)malloc(1024);
	snprintf(buf, 1024, configFile, dark_theme);
	
	if (R_FAILED(ret = writeFile("/3ds/data/3dstool/config.cfg", buf)))
		return ret;
	
	free(buf);
	return 0;
}	

Result loadConfig(void)
{
	Handle handle;
	Result ret = 0;
	
	if (!fileExists(fsArchive, "/3ds/data/3dstool/config.cfg"))
	{
		darkTheme = false;
		return saveConfig(darkTheme);
	}
	
	if (R_FAILED(ret = FSUSER_OpenFileDirectly(&handle, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, "/3ds/data/3dstool/config.cfg"), FS_OPEN_READ, 0)))
		return ret;
	
	u64 size64 = 0;
	u32 size = 0;
	
	if (R_FAILED(ret = FSFILE_GetSize(handle, &size64)))
		return ret;
		
	size = (u32)size64;
	
	char * buf = (char *)malloc(size + 1);
	u32 bytesread = 0;
	
	if (R_FAILED(ret = FSFILE_Read(handle, &bytesread, 0, (u32 *)buf, size)))
		return ret;
	
	buf[size] = '\0';
	
	sscanf(buf, configFile, &darkTheme);
	
	if (R_FAILED(ret = FSFILE_Close(handle)))
		return ret;
	
	free(buf);
	return 0;
}

void makeDirs(void)
{
	if (!(dirExists(fsArchive, "/3ds/")))
		makeDir(fsArchive, "/3ds");
	if (!(dirExists(fsArchive, "/3ds/data/")))
		makeDir(fsArchive, "/3ds/data");
	if (!(dirExists(fsArchive, "/3ds/data/3dstool/")))
		makeDir(fsArchive, "/3ds/data/3dstool");
	if (!(dirExists(fsArchive, "/3ds/data/3dstool/backups/")))
		makeDir(fsArchive, "/3ds/data/3dstool/backups");
	if (!(dirExists(fsArchive, "/3ds/data/3dstool/dumps/")))
		makeDir(fsArchive, "/3ds/data/3dstool/dumps");
	if (!(dirExists(fsArchive, "/3ds/data/3dstool/backups/nand/")))
	{
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/ro");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/rw");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/ro/sys");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/rw/sys");
		makeDir(fsArchive, "/3ds/data/3dstool/backups/nand/private");
	}
}

bool isN3DS(void)
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