#include <stdio.h>
#include <stdlib.h>
#include <errorcodes.h>
#include <string.h>
#include <errno.h>
#include <eapi.h>
#include <common.h>


int initialize_gpio(void);

uint32_t EApiLibInitialize(void)
{
	char res[128];
	char sysfile[128];
	volatile int ret;

	initialize_gpio();
	
	sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/board_name");
	ret = read_sysfs_file(sysfile, res, sizeof(res));
	if(ret < 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}	

	if (strlen(res) <= 1)
		return EAPI_STATUS_UNSUPPORTED;

	return EAPI_STATUS_SUCCESS;


}

uint32_t EApiLibUnInitialize(void)
{

	return EAPI_STATUS_SUCCESS;
}

