#include <stdlib.h>
#include <string.h>

#include "fs.h"

Result openArchive(FS_Archive * archive, FS_ArchiveID archiveID)
{
	Result ret = 0;
	
	if (R_FAILED(ret = FSUSER_OpenArchive(archive, archiveID, fsMakePath(PATH_EMPTY, ""))))
		return ret;
	
	return 0;
}

Result closeArchive(FS_Archive archive)
{
	Result ret = 0;
	
	if (R_FAILED(ret = FSUSER_CloseArchive(archive)))
		return ret;
	
	return 0;
}

Result makeDir(FS_Archive archive, const char * path)
{
	if (!path)
		return -1;
	
	Result ret = 0;
	
	if (R_FAILED(ret = FSUSER_CreateDirectory(archive, fsMakePath(PATH_ASCII, path), 0)))
		return ret;
	
	return 0;
}

bool fileExists(FS_Archive archive, const char * path)
{
	if (!path)
		return false;
	
	Handle handle;
	
	Result ret = 0;
	
	if (R_FAILED(ret = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0)))
		return false;

	if (R_FAILED(ret = FSFILE_Close(handle)))
		return false;
	
	return true;
}

bool dirExists(FS_Archive archive, const char * path)
{	
	if ((!path) || (!archive))
		return false;
	
	Handle handle;

	Result ret = 0;
	
	if (R_FAILED(ret = FSUSER_OpenDirectory(&handle, archive, fsMakePath(PATH_ASCII, path))))
		return false;
	
	if (R_FAILED(ret = FSDIR_Close(handle)))
		return false;
	
	return true;
}

u64 getFileSize(FS_Archive archive, const char * path)
{
	u64 st_size;
	Handle handle;

	FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0);
	FSFILE_GetSize(handle, &st_size);
	FSFILE_Close(handle);
	
	return st_size;
}

Result writeFile(FS_Archive archive, FS_ArchiveID archiveID, const char * path, void * buf)
{
	Handle handle;
	u32 len = strlen(buf);
	u64 size;
	u32 written;
	
	if (fileExists(archive, path))
		FSUSER_DeleteFile(archive, fsMakePath(PATH_ASCII, path));
	
	Result ret = FSUSER_OpenFileDirectly(&handle, archiveID, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, path), (FS_OPEN_WRITE | FS_OPEN_CREATE), 0);
	ret = FSFILE_GetSize(handle, &size);
	ret = FSFILE_SetSize(handle, size + len);
	ret = FSFILE_Write(handle, &written, size, buf, len, FS_WRITE_FLUSH);
	ret = FSFILE_Close(handle);
	
	return R_SUCCEEDED(ret)? 0 : -1;
}

Result copy_file(FS_Archive srcArchive, FS_Archive destArchive, FS_ArchiveID srcArchiveID, FS_ArchiveID destArchiveID, char * src, char * dest)
{
	int chunksize = (512 * 1024);
	char * buffer = (char *)malloc(chunksize);

	u32 bytesWritten = 0, bytesRead = 0;
	u64 offset = 0;
	Result ret = 0;
	
	Handle inputHandle, outputHandle;

	Result in = FSUSER_OpenFileDirectly(&inputHandle, srcArchiveID, fsMakePath(PATH_EMPTY, ""), fsMakePath(PATH_ASCII, src), FS_OPEN_READ, 0);
	
	u64 size = getFileSize(srcArchive, src);

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