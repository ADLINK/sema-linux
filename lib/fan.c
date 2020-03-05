#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <eapi.h>
#include <dirent.h>


int get_hwmon_num(void);


uint32_t EApiSmartFanSetTempSetpoints(int id, int Level1, int Level2, int Level3, int Level4)
{
	errno = 0;
	char fan_sysfile[512];
	int i;
	int fan_no;
	char buff[256];
	int Level_val;


	/*Check inputs are valid*/
	if((Level1 < -127) || (Level1 > 128) || (Level2 < - 127) || (Level2 > 128) || (Level3 < -127) || (Level3 > 128) || (Level4 < -127) || (Level4 > 128) || (id < 0) || (id > 4))
	{
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
		return -1;

	for(i=1;i<=4;i++)
	{
		FILE *fp;
		sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_auto_point%d_temp", fan_no, (id+1), i);
		fp = fopen (fan_sysfile, "w");
		if (fp == NULL)
			return -1;
		if(i==1)
			Level_val = Level1;
		else if(i==2)
			Level_val = Level2;
		else if(i==3)
			Level_val = Level3;
		else if(i==4)
			Level_val = Level4;

		sprintf(buff, "%d", Level_val);
		int ret;
		ret = fwrite(buff, 4, sizeof(char), fp);
		if (ret)
			fclose(fp);
		else {
			fclose(fp);
			return -1;
		}
	}

	return 0;

}

uint32_t EApiSmartFanGetTempSetpoints(int id, int *pLevel1, int *pLevel2, int *pLevel3, int *pLevel4)
{
	char fan_sysfile[512];
	int i;
	int fan_no;
	char buff[256];

	/*Check inputs are valid*/
	if((id < 0) || (id >= 4))
	{
		errno = EINVAL;
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
		return -1;


	for(i=1;i<=4;i++)
	{
		FILE *fp;
		sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_auto_point%d_temp", fan_no, (id+1), i);
		fp = fopen (fan_sysfile, "r");
		if (fp == NULL)
			return -1;
	
		int ret;
		ret = fread(buff, sizeof(char), 256, fp);
		if (ret){
			fclose(fp);
		}
		else {
			fclose(fp);
			return -1;
		}
		if(i==1)
			*pLevel1 = (char) atoi(buff);
		else if(i==2)
			*pLevel2 = (char) atoi(buff);
		else if(i==3)
			*pLevel3 = (char) atoi(buff);
		else if(i==4)
			*pLevel4 = (char) atoi(buff);

			
			
	}
	return 0;


}

uint32_t EApiSmartFanSetPWMSetpoints(int id, int pwm_Level1, int pwm_Level2, int pwm_Level3, int pwm_Level4)
{
	char fan_sysfile[512];
	int i;
	int fan_no;
	char buff[256];
	int Level_val;


	/*Check inputs are valid*/
	if((pwm_Level1 < 0) || (pwm_Level1 > 100) || (pwm_Level2 < 0) || (pwm_Level2 > 100) || (pwm_Level3 < 0) || (pwm_Level3 > 100) || (pwm_Level4 < 0) || (pwm_Level4 > 100) || (id < 0) || (id > 4))
	{
		errno = EINVAL;
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
		return -1;


	for(i=1;i<=4;i++)
	{
		FILE* fp;
		int ret;
		sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_auto_point%d_pwm", fan_no, (id+1), i);
		fp = fopen (fan_sysfile, "w+");
		if (fp == NULL)
			return -1;
		if(i==1)
			Level_val = pwm_Level1;
		else if(i==2)
			Level_val = pwm_Level2;
		else if(i==3)
			Level_val = pwm_Level3;
		else if(i==4)
			Level_val = pwm_Level4;

		sprintf(buff, "%d", Level_val);
		ret = fwrite(buff, 4, sizeof(char), fp);
		if (ret)
			fclose(fp);
		else {
			fclose(fp);
			return -1;
		}
	}

	return 0;

}

uint32_t EApiSmartFanGetPWMSetpoints(int id, int *pLevel1, int *pLevel2, int *pLevel3, int *pLevel4)
{
	char fan_sysfile[512];
	int i;
	int fan_no;
	char buff[256];


	/*Check inputs are valid*/
	if((pLevel1 == NULL) || (pLevel2 == NULL) || (pLevel3 == NULL) || (pLevel4 == NULL) || (id < 0) || (id >= 4))
	{
		errno = EINVAL;
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
		return -1;


	for(i=1;i<=4;i++)
	{
		FILE *fp;
		int ret; 
		sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_auto_point%d_pwm", fan_no, (id+1), i);
		fp = fopen (fan_sysfile, "r");
		if (fp == NULL)
			return -1;

		ret = fread(buff, sizeof(char), 256, fp);
		if (ret)
			fclose(fp);
		else {
			fclose(fp);
			return -1;
		}
		if(i==1)
			*pLevel1 = atoi(buff);
		else if(i==2)
			*pLevel2 = atoi(buff);
		else if(i==3)
			*pLevel3 = atoi(buff);
		else if(i==4)
			*pLevel4 = atoi(buff);
	}
	return 0;

}

uint32_t EApiSmartFanGetMode(int id, int *fan_mode)
{
	int ret = 0;
	char fan_sysfile[512];
	FILE* fp;
	int fan_no;
	char buff[256];


	/*Check inputs are valid*/
	if((fan_mode == NULL) || (id < 0) || (id >= 4))
	{
		errno = EINVAL;
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
		return -1;


	sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_enable", fan_no, (id+1));
	fp = fopen (fan_sysfile, "r");
	if (fp == NULL)
		return -1;

	ret = fread(buff, sizeof(char), 256, fp);
	if (ret)
		fclose(fp);
	else {
		fclose(fp);
		return -1;
	}

	*fan_mode = atoi(buff);
	
	return 0;

}

uint32_t EApiSmartFanSetMode(int id, int fan_mode)
{
	int ret = 0;
	char fan_sysfile[512];
	FILE *fp;
	int fan_no;
	char buff[256];


	/*Check inputs are valid*/
	if((fan_mode < 0) || (fan_mode >= 4)  || (id < 0) || (id >= 4))
	{
		errno = EINVAL;
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
		return -1;


	sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_enable", fan_no, (id+1));
	fp = fopen (fan_sysfile, "w+");
	if (fp == NULL)
		return -1;

	sprintf(buff, "%d", fan_mode);
	ret = fwrite(buff, 4, sizeof(char), fp);
	if (ret)
		fclose(fp);
	else {
		fclose(fp);	
		return -1;
	}
			
	return 0;

}

uint32_t EApiSmartFanGetTempSrc(int id, int *pTempsrc)
{
	int ret = 0;
	char fan_sysfile[512];
	FILE* fp;
	int fan_no;
	char buff[256];

	/*Check inputs are valid*/
	if((pTempsrc == NULL) || (id < 0) || (id >= 4))
	{
		errno = EINVAL;
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
		return -1;

	sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_auto_channels_temp", fan_no, (id+1));
	fp = fopen (fan_sysfile, "r");
	if (fp == NULL)
		return -1;

	ret = fread(buff, sizeof(char), 256, fp);
	if (ret)
		fclose(fp);
	else {
		fclose(fp);
		return -1;
	}
	*pTempsrc = atoi(buff);
	
	return 0;
}

uint32_t EApiSmartFanSetTempSrc(int id, int Tempsrc)
{
        int ret = 0;
	char fan_sysfile[512];
	FILE* fp;
	int fan_no;
	char buff[256];

	/*Check inputs are valid*/
	if((Tempsrc < 0) || (Tempsrc > 1)  || (id < 0) || (id >= 4))
	{
		errno = EINVAL;
		return -1;
	}

	/*Check whether FAN driver is loaded*/
	fan_no = get_hwmon_num();
	if (fan_no < 0)
	{
		printf("Fan driver not loaded\n");
		return -1;
	}


	sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/fan%d_auto_channels_temp", fan_no, (id+1));
	fp = fopen (fan_sysfile, "w+");
	if (fp == NULL)
		return -1;

	sprintf(buff, "%d", Tempsrc);
	ret = fwrite(buff, 4, sizeof(char), fp);
	if (ret)
		fclose(fp);
	else {
		fclose(fp);
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
		sprintf(fan_sysfile, "/sys/class/hwmon/hwmon%d/device/driver/adl-bmc-fan/", fan_no);
		dir = opendir(fan_sysfile);
		if(dir)
		{
			closedir(dir);
			return fan_no;
		}
		
			
	}

	return -1;

}
