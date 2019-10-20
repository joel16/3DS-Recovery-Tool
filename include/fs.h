#ifndef _3DS_RECOVERY_TOOL_FS_H
#define _3DS_RECOVERY_TOOL_FS_H

#include <3ds.h>

extern FS_Archive sdmc_archive, nand_archive;

Result FS_OpenArchive(FS_Archive *archive, FS_ArchiveID id);
Result FS_CloseArchive(FS_Archive archive);
Result FS_MakeDir(FS_Archive archive, const char *path);
Result FS_RecursiveMakeDir(FS_Archive archive, const char *path);
bool FS_FileExists(FS_Archive archive, const char *path);
bool FS_DirExists(FS_Archive archive, const char *path);
Result FS_GetFileSize(FS_Archive archive, const char *path, u64 *size);
Result FS_Write(FS_Archive archive, const char *path, const void *buf, u32 size);
Result FS_CopyFile(FS_Archive src_archive, FS_Archive dest_archive, char *src_path, char *dest_path);

#endif
