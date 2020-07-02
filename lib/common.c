// SPDX-License-Identifier: LGPL-2.0+
/*
 * Helper functions for SEMA Library
 *
 * Copyright (C) 2020 ADLINK Technology Inc.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common.h>
#include <eapi.h>
#include <dirent.h>



int read_sysfs_file(char *sysfile, char *value, unsigned short size)
{	
	int fd, ret;

	fd = open(sysfile, O_RDONLY);
	if(fd < 0)
		return -1;

	ret = read(fd, value, size);	
	if (ret)
		close(fd);
	else {
		close(fd);
		return -1;
	}

	return 0;
}
int write_sysfs_file(char *sysfile, char *value, unsigned short size)
{	
	int fd, ret;

	fd = open(sysfile, O_RDWR);
	if(fd < 0){
		return -1;
	}

	ret = write(fd, value, size);	
	if (ret)
		close(fd);
	else {
		close(fd);
		return -1;
	}

	return 0;
}

int get_hwmon_num(void)
{
	DIR *dir;
	uint32_t max_hwmon;
	char fan_sysfile[512];
	int fan_no;

	for (max_hwmon=0;;max_hwmon++)
	{
		sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%u", max_hwmon);
		dir = opendir(fan_sysfile);
		if(dir)
			closedir(dir);
		else
			break;

	}


	for (fan_no=0;fan_no <= max_hwmon; fan_no++)
	{
		sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/driver/adl-bmc-hwmon/", fan_no);
		dir = opendir(fan_sysfile);
		if(dir)
		{
			closedir(dir);
			return fan_no;
		}
		
			
	}

	return -1;

}




