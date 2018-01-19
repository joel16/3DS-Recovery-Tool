#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"

Result FS_OpenArchive(FS_Archive * archive, FS_ArchiveID archiveID)
{
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_OpenArchive(archive, archiveID, fsMakePath(PATH_EMPTY, ""))))
		return ret;

	return 0;
}

Result FS_CloseArchive(FS_Archive archive)
{
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_CloseArchive(archive)))
		return ret;

	return 0;
}

Result FS_MakeDir(FS_Archive archive, const char * path)
{	
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_CreateDirectory(archive, fsMakePath(PATH_ASCII, path), 0)))
		return ret;
	
	return 0;
}

void FS_RecursiveMakeDir(FS_Archive archive, const char * dir) 
{
	char tmp[256];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp), "%s",dir);
	len = strlen(tmp);

	if (tmp[len - 1] == '/')
		tmp[len - 1] = 0;

	for (p = tmp + 1; *p; p++)
	{
		if (*p == '/') 
		{
			*p = 0;
			FS_MakeDir(archive, tmp);
			*p = '/';
		}
		FS_MakeDir(archive, tmp);
	}
}

bool FS_FileExists(FS_Archive archive, const char * path)
{
	Handle handle;

	if (R_FAILED(FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0)))
		return false;

	if (R_FAILED(FSFILE_Close(handle)))
		return false;

	return true;
}

bool FS_DirExists(FS_Archive archive, const char * path)
{
	Handle handle;

	if (R_FAILED(FSUSER_OpenDirectory(&handle, archive, fsMakePath(PATH_ASCII, path))))
		return false;

	if (R_FAILED(FSDIR_Close(handle)))
		return false;

	return true;
}

u64 FS_GetFileSize(FS_Archive archive, const char * path)
{
	Handle handle;
	Result ret = 0;
	u64 st_size = 0;

	if (R_FAILED(ret = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0)))
		return ret;

	if (R_FAILED(ret = FSFILE_GetSize(handle, &st_size)))
		return ret;

	if (R_FAILED(ret = FSFILE_Close(handle)))
		return ret;

	return st_size;
}

Result FS_WriteFile(FS_Archive archive, FS_ArchiveID archiveID, const char * path, void * buf)
{
	Handle handle;
	Result ret = 0;

	u32 len = strlen(buf);
	u64 size;
	u32 written;
	
	if (FS_FileExists(archive, path))
		FSUSER_DeleteFile(archive, fsMakePath(PATH_ASCII, path));

	if (R_FAILED(ret = FSUSER_OpenFileDirectly(&handle, archiveID, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, path), (FS_OPEN_WRITE | FS_OPEN_CREATE), 0)))
		return ret;

	if (R_FAILED(ret = FSFILE_GetSize(handle, &size)))
		return ret;

	if (R_FAILED(ret = FSFILE_SetSize(handle, size + len)))
		return ret;

	if (R_FAILED(ret = FSFILE_Write(handle, &written, size, buf, len, FS_WRITE_FLUSH)))
		return ret;

	if (R_FAILED(ret = FSFILE_Close(handle)))
		return ret;
	
	return 0;
}

Result FS_Copy_File(FS_Archive srcArchive, FS_Archive destArchive, FS_ArchiveID srcArchiveID, FS_ArchiveID destArchiveID, char * src, char * dest)
{
	int chunksize = (512 * 1024);
	char * buffer = (char *)malloc(chunksize);

	u32 bytesWritten = 0, bytesRead = 0;
	u64 offset = 0;
	Result ret = 0;
	
	Handle inputHandle, outputHandle;

	Result in = FSUSER_OpenFileDirectly(&inputHandle, srcArchiveID, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, src), FS_OPEN_READ, 0);
	
	u64 size = FS_GetFileSize(srcArchive, src);

	if (R_SUCCEEDED(in))
	{
		// Delete output file (if existing)
		FSUSER_DeleteFile(destArchive, fsMakePath(PATH_ASCII, dest));

		Result out = FSUSER_OpenFileDirectly(&outputHandle, destArchiveID, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, dest), (FS_OPEN_CREATE | FS_OPEN_WRITE), 0);
		
		if (R_SUCCEEDED(out))
		{
			// Copy loop (512KB at a time)
			do
			{
				ret = FSFILE_Read(inputHandle, &bytesRead, offset, buffer, chunksize);
				
				bytesWritten += FSFILE_Write(outputHandle, &bytesWritten, offset, buffer, size, FS_WRITE_FLUSH);
				
				if (bytesWritten == bytesRead)
					break;
			}
			while(bytesRead);

			ret = FSFILE_Close(outputHandle);
			
			if (bytesRead != bytesWritten) 
				return ret;
		}
		else 
			return out;

		FSFILE_Close(inputHandle);
	}
	else 
		return in;

	free(buffer);
	return 0;
}