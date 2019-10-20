#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"

FS_Archive sdmc_archive, nand_archive;

Result FS_OpenArchive(FS_Archive *archive, FS_ArchiveID archiveID) {
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_OpenArchive(archive, archiveID, fsMakePath(PATH_EMPTY, ""))))
		return ret;

	return 0;
}

Result FS_CloseArchive(FS_Archive archive) {
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_CloseArchive(archive)))
		return ret;

	return 0;
}

Result FS_MakeDir(FS_Archive archive, const char *path) {
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_CreateDirectory(archive, fsMakePath(PATH_ASCII, path), 0)))
		return ret;
	
	return 0;
}

Result FS_RecursiveMakeDir(FS_Archive archive, const char *path) {
	Result ret = 0;
	char buf[256];
	char *p = NULL;
	size_t len;

	snprintf(buf, sizeof(buf), "%s", path);
	len = strlen(buf);

	if (buf[len - 1] == '/')
		buf[len - 1] = 0;

	for (p = buf + 1; *p; p++) {
		if (*p == '/') {
			*p = 0;

			if (!FS_DirExists(archive, buf))
				ret = FS_MakeDir(archive, buf);
			
			*p = '/';
		}
		
		if (!FS_DirExists(archive, buf))
			ret = FS_MakeDir(archive, buf);
	}
	
	return ret;
}

bool FS_FileExists(FS_Archive archive, const char *path) {
	Handle handle;

	if (R_FAILED(FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0)))
		return false;

	if (R_FAILED(FSFILE_Close(handle)))
		return false;

	return true;
}

bool FS_DirExists(FS_Archive archive, const char *path) {
	Handle handle;

	if (R_FAILED(FSUSER_OpenDirectory(&handle, archive, fsMakePath(PATH_ASCII, path))))
		return false;

	if (R_FAILED(FSDIR_Close(handle)))
		return false;

	return true;
}

Result FS_GetFileSize(FS_Archive archive, const char *path, u64 *size) {
	Result ret = 0;
	Handle handle;

	if (R_FAILED(ret = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0)))
		return ret;

	if (R_FAILED(ret = FSFILE_GetSize(handle, size))) {
		FSFILE_Close(handle);
		return ret;
	}

	if (R_FAILED(ret = FSFILE_Close(handle)))
		return ret;
	
	return 0;
}

static Result FS_RemoveFile(FS_Archive archive, const char *path) {
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_DeleteFile(archive, fsMakePath(PATH_ASCII, path))))
		return ret;
	
	return 0;
}

Result FS_Write(FS_Archive archive, const char *path, const void *buf, u32 size) {
	Result ret = 0;
	Handle handle;
	u32 bytes_written = 0;

	if (FS_FileExists(archive, path)) {
		if (R_FAILED(ret = FS_RemoveFile(archive, path)))
			return ret;
	}

	if (R_FAILED(ret = FSUSER_CreateFile(archive, fsMakePath(PATH_ASCII, path), 0, size)))
		return ret;

	if (R_FAILED(ret = FSUSER_OpenFile(&handle, archive, fsMakePath(PATH_ASCII, path), FS_OPEN_WRITE, 0)))
		return ret;
	
	if (R_FAILED(ret = FSFILE_Write(handle, &bytes_written, 0, buf, size, FS_WRITE_FLUSH))) {
		FSFILE_Close(handle);
		return ret;
	}

	if (R_FAILED(ret = FSFILE_Close(handle)))
		return ret;

	return 0;
}

Result FS_CopyFile(FS_Archive src_archive, FS_Archive dest_archive, char *src_path, char *dest_path) {
	Handle src_handle, dst_handle;
	Result ret = 0;

	if (R_FAILED(ret = FSUSER_OpenFile(&src_handle, src_archive, fsMakePath(PATH_ASCII, src_path), FS_OPEN_READ, 0)))
		return ret;
	
	if (R_FAILED(ret = FSUSER_OpenFile(&dst_handle, dest_archive, fsMakePath(PATH_ASCII, dest_path), FS_OPEN_CREATE | FS_OPEN_WRITE, 0))) {
		FSFILE_Close(src_handle);
		return ret;
	}

	u32 bytes_read = 0;
	u64 offset = 0, size = 0;
	size_t buf_size = 0x10000;
	u8 *buf = malloc(buf_size); // Chunk size
	FS_GetFileSize(src_archive, src_path, &size);

	do {
		memset(buf, 0, buf_size);

		if (R_FAILED(ret = FSFILE_Read(src_handle, &bytes_read, offset, buf, buf_size))) {
			free(buf);
			FSFILE_Close(src_handle);
			FSFILE_Close(dst_handle);
			return ret;
		}
		if (R_FAILED(ret = FSFILE_Write(dst_handle, NULL, offset, buf, bytes_read, FS_WRITE_FLUSH))) {
			free(buf);
			FSFILE_Close(src_handle);
			FSFILE_Close(dst_handle);
			return ret;
		}

		offset += bytes_read;
	}
	while(offset < size);

	free(buf);
	FSFILE_Close(src_handle);
	FSFILE_Close(dst_handle);
	return 0;
}
