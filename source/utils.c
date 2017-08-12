#include "fs.h"
#include "utils.h"

bool isN3DS(void)
{
	bool isNew3DS = 0;
	APT_CheckNew3DS(&isNew3DS);
    
	if (isNew3DS)
		return true;
	else
		return false;
}

void setConfig(const char * path, bool set) // using individual txt files for configs for now (plan to change this later when there's more options)
{
	if (set == true)
		writeFile(path, "1");
	else
		writeFile(path, "0");
}