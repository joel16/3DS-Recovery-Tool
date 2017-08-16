#include "fs.h"
#include "utils.h"

bool isN3DS(void)
{
	bool isNew3DS = false;
	
	if (R_SUCCEEDED(APT_CheckNew3DS(&isNew3DS)))
		return isNew3DS;
	
	return false;
}

Result setConfig(const char * path, bool set) // using individual txt files for configs for now (plan to change this later when there's more options)
{
	if (set)
		return writeFile(path, "1");
	
	return writeFile(path, "0");
}