#ifndef __COMMON_H__
#define __COMMON_H__


int read_sysfs_file(char *sysfile, char *value, unsigned short size);
int write_sysfs_file(char *sysfile, char *value, unsigned short size);

int get_hwmon_num(void);


#endif
