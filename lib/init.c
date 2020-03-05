#include <stdio.h>
#include <stdlib.h>
#include <errorcodes.h>
#include <string.h>
#include <errno.h>
#include <eapi.h>
#include <common.h>

uint32_t EApiLibInitialize(void)
{
	char res[128];
	char sysfile[128];
	int ret;
	
	sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/board_name");
	ret = read_sysfs_file(sysfile, res, sizeof(res));
	if(ret < 0) {
		return ret;
	}	

	if (strlen(res) <= 1)
		return -1;

	return 0;

}

uint32_t EApiLibUnInitialize(void)
{

	return 0;
}

