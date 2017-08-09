#include <stdlib.h>
#include <string.h>

#include "fs.h"

void openArchive(FS_ArchiveID id)
{
	FSUSER_OpenArchive(&fsArchive, id, fsMakePath(PATH_EMPTY, ""));
}

void closeArchive(FS_ArchiveID id)
{
	FSUSER_CloseArchive(fsArchive);
}

Result makeDir(FS_Archive archive, const char * path)
{
	if((!archive) || (!path))
		return -1;
	
	return FSUSER_CreateDirectory(archive, fsMakePath(PATH_ASCII, path), 0);
}

bool fileExists(FS_Archive archive, const char * path)
{
	if((!path) || (!archive))
		return false;
	
	Handle handle;

	Result ret = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0);
	
	if(ret != 0)
		return false;

	ret = FSFILE_Close(handle);
	
	if(ret != 0)
		return false;
	
	return true;
}

bool dirExists(FS_Archive archive, const char * path)
{	
	if((!path) || (!archive))
		return false;
	
	Handle handle;

	Result ret = FSUSER_OpenDirectory(&handle, archive, fsMakePath(PATH_ASCII, path));
	
	if(ret != 0)
		return false;

	ret = FSDIR_Close(handle);
	
	if(ret != 0)
		return false;
	
	return true;
}

Result copy_file_archive(const char * src, const char * dst)
{
	u64 size = 0;
	u32 bytesRead = 0;
	
	openArchive(ARCHIVE_NAND_CTR_FS);
	
	Handle handle;
	Result res = FSUSER_OpenFile(&handle, fsArchive, fsMakePath(PATH_ASCII, src), FS_OPEN_READ, 0);
	
	if (R_FAILED(res)) 
		return res;
	
	else if (R_SUCCEEDED(res))
    {
        FSFILE_GetSize(handle, &size);
		
		char * buf = malloc(size);
		memset(buf, 0, size);
		
		FSFILE_Read(handle, &bytesRead, 0, buf, size);
		FSFILE_Close(handle);
		closeArchive(ARCHIVE_NAND_CTR_FS);
		
		FSUSER_CreateFile(fsArchive, fsMakePath(PATH_ASCII, dst), 0, size);
		FSUSER_OpenFile(&handle, fsArchive, fsMakePath(PATH_ASCII, dst), FS_OPEN_WRITE, 0);
		Result res = FSFILE_Write(handle, &bytesRead, 0, buf, size, FS_WRITE_FLUSH);
		
		if (R_FAILED(res))
			return res;
		
		free(buf);
		FSFILE_Close(handle);
    }
	
	return res;
}