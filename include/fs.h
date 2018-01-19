#ifndef FS_H
#define FS_H

#include <3ds.h>

FS_Archive sdmcArchive, nandArchive;

Result FS_OpenArchive(FS_Archive * archive, FS_ArchiveID archiveID);
Result FS_CloseArchive(FS_Archive archive);
Result FS_MakeDir(FS_Archive archive, const char * path);
void FS_RecursiveMakeDir(FS_Archive archive, const char * dir);
bool FS_FileExists(FS_Archive archive, const char * path);
bool FS_DirExists(FS_Archive archive, const char * path);
u64 FS_GetFileSize(FS_Archive archive, const char * path);
Result FS_WriteFile(FS_Archive archive, FS_ArchiveID archiveID, const char * path, void * buf);
Result FS_Copy_File(FS_Archive srcArchive, FS_Archive destArchive, FS_ArchiveID srcArchiveID, FS_ArchiveID destArchiveID, char * src, char * dest);

#endif