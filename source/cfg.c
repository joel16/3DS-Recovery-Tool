#include "cfg.h"

static Handle cfguHandle;

Result CFGI_RestoreNANDLocalFriendCodeSeed(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x80D, 0, 0); // 0x080D0000

	if (R_FAILED(ret = svcSendSyncRequest(cfguHandle)))
		return ret;

	return (Result)cmdbuf[1];
}

Result CFGI_RestoreNANDSecureInfo(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x812, 0, 0); // 0x08120000

	if (R_FAILED(ret = svcSendSyncRequest(cfguHandle)))
		return ret;

	return (Result)cmdbuf[1];
}

Result CFGI_FormatConfig(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x806, 0, 0); // 0x08060000

	if (R_FAILED(ret = svcSendSyncRequest(cfguHandle)))
		return ret;

	return (Result)cmdbuf[1];
}