#include "cfg.h"

static Handle cfgiHandle;

Result cfgiInit(void)
{
	return srvGetServiceHandle(&cfgiHandle, "cfg:i");
}

Result cfgiExit(void)
{
    return svcCloseHandle(cfgiHandle);
}

Result CFGI_RestoreNANDLocalFriendCodeSeed(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x80D, 0, 0); // 0x080D0000

	if (R_FAILED(ret = svcSendSyncRequest(cfgiHandle)))
		return ret;

	return cmdbuf[1];
}

Result CFGI_RestoreNANDSecureInfo(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x812, 0, 0); // 0x08120000

	if (R_FAILED(ret = svcSendSyncRequest(cfgiHandle)))
		return ret;

	return cmdbuf[1];
}

Result CFGI_DeleteConfigNANDSavefile(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x805, 0, 0); // 0x08050000

	if (R_FAILED(ret = svcSendSyncRequest(cfgiHandle)))
		return ret;

	return cmdbuf[1];
}

Result CFGI_FormatConfig(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x806, 0, 0); // 0x08060000

	if (R_FAILED(ret = svcSendSyncRequest(cfgiHandle)))
		return ret;

	return cmdbuf[1];
}

Result CFGI_ClearParentalControls(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x40F, 0, 0); // 0x040F0000

	if (R_FAILED(ret = svcSendSyncRequest(cfgiHandle)))
		return ret;

	return cmdbuf[1];
}

Result CFGI_VerifySigLocalFriendCodeSeed(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x80E, 0, 0); // 0x080E0000

	if (R_FAILED(ret = svcSendSyncRequest(cfgiHandle)))
		return ret;

	return cmdbuf[1];
}

Result CFGI_VerifySigSecureInfo(void) 
{
	Result ret = 0;
	u32 * cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x813, 0, 0); // 0x08130000

	if (R_FAILED(ret = svcSendSyncRequest(cfgiHandle)))
		return ret;

	return cmdbuf[1];
}