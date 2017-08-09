#ifndef CFG_H
#define CFG_H

#include <3ds.h>

Result CFGI_RestoreNANDLocalFriendCodeSeed(void);
Result CFGI_RestoreNANDSecureInfo(void);
Result CFGI_DeleteConfigNANDSavefile(void);
Result CFGI_FormatConfig(void);

#endif