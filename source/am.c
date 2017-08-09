#include "am.h"

static Handle amHandle;

Result AM_DeleteAllTemporaryTitles(void)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x16,0,0); // 0x00160000

	if(R_FAILED(ret = svcSendSyncRequest(amHandle))) return ret;

	return cmdbuf[1];
}

Result AM_DeleteAllExpiredTitles(FS_MediaType mediatype)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x1F,1,0); // 0x001F0040
	cmdbuf[1] = mediatype;

	if(R_FAILED(ret = svcSendSyncRequest(amHandle))) return ret;

	return cmdbuf[1];
}

Result AM_DeleteAllTwlTitles(void)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = IPC_MakeHeader(0x1D,0,0); // 0x001D0000

	if(R_FAILED(ret = svcSendSyncRequest(amHandle))) return ret;

	return cmdbuf[1];
}