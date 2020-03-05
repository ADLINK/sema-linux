#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <common.h>
#include <eapi.h>



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


