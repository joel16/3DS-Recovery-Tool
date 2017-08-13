#ifndef CFG_H
#define CFG_H

#include <3ds.h>

Result cfgiInit(void);
Result cfgiExit(void);
Result CFGI_RestoreNANDLocalFriendCodeSeed(void);
Result CFGI_RestoreNANDSecureInfo(void);
Result CFGI_DeleteConfigNANDSavefile(void);
Result CFGI_FormatConfig(void);
Result CFGI_ClearParentalControls(void);
Result CFGI_VerifySigLocalFriendCodeSeed(void);
Result CFGI_VerifySigSecureInfo(void);

#endif