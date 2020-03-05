#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <dirent.h> 
#include <string.h>
#include <eapi.h>
#include <common.h>
#include <unistd.h>

#define KELVINS_OfFSET 2731

#define PLATFORMS_NUMBER 2

char *Board[PLATFORMS_NUMBER] = {
	"LEC-AL",
	"Q7-AL"
};


static int encode_celcius(uint32_t temp)
{
	uint32_t tmp;
	tmp = temp / 1000;
        tmp = tmp * 10 + KELVINS_OfFSET;
        return tmp;
}

uint32_t IsFileExist(char *sysf)
{
	int fd;
	fd = open(sysf, O_RDONLY);
	if (fd < 1)
		return -1;

	return 0;

}

uint32_t EApiBoardGetStringA(uint32_t Id, char *pBuffer, uint32_t *pBufLen)
{

	char res[128];
	memset(res, 0, 128);
	char sysfile[128];
	int ret;

	uint32_t status = EAPI_STATUS_SUCCESS;

	switch (Id)
	{
		case 1:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/manufacturer_name");
			break;
		case 2:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/board_name");
			break;
		case 3:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/serial_number");
			break;
		case 4:
			sprintf(sysfile, "/sys/class/dmi/id/bios_version");
			break;
		case 5:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/bmc_boot_version");
			break;
		case 6:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/restart_event_str");
			break;
		case 7:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/hw_rev");
			break;
		case 8:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/bmc_application_version");
			break;
		case 9:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/last_repair_date");
			break;
		case 10:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/manufactured_date");
			break;
		case 11:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/mac_address");
			break;
		case 12:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/second_hw_rev");
			break;
		case 13:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/second_ser_num");
			break;
		default:
			status = EAPI_STATUS_UNSUPPORTED;
	}

	ret = read_sysfs_file(sysfile, pBuffer, *pBufLen);
	
	if (ret == 0)
		status = EAPI_STATUS_SUCCESS;

	if (strlen(pBuffer) == 0 || ret == -1){
		return EAPI_STATUS_UNSUPPORTED;
	}

	return status;

}

uint32_t EApiBoardGetValue(uint32_t Id, uint32_t *pValue)
{

	char res[128];
	memset(res, 0, 128);
	char sysfile[128] = {0};
	int ret;
	uint32_t status = EAPI_STATUS_SUCCESS;

	switch (Id)
	{
		case 1:
			*pValue = (EAPI_VERSION);
			break;
		case 2:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/boot_counter_val");
			break;
		case 3:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/total_up_time");
			break;
		case 4:
			*pValue = EAPI_VER_CREATE(3, 5, 9);
			break;
		case 5:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/cpu_cur_temp");
			ret = IsFileExist(sysfile);
			if (ret){
				sprintf(sysfile, "/sys/class/thermal/thermal_zone1/temp");
			}
		
			break;
		case 6:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys1_cur_temp");
			break;
		case 7:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_vcore");
			break;
		case 8:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_2v5");
			break;
		case 9:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_3v3");
			break;
		case 10:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_vbat");
			break;
		case 11:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_5v");
			break;
		case 12:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_5vsb");
			break;
		case 13:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_12v");
			break;
		case 14:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/cpu_fan_speed");
			break;
		case 15:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys1_fan_speed");
			break;
		case 16:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/power_up_time");
			break;
		case 17:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/restart_event");
			break;
		case 18:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/capabilities");
			break;
		case 19:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/capabilities_ext");
			break;
		case 20:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys1_min_temp");
			break;
		case 21:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys1_max_temp");
			break;
		case 22:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys1_startup_temp");
			break;
		case 23:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/cpu_min_temp");
			break;
		case 24:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/cpu_max_temp");
			break;
		case 25:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/cpu_startup_temp");
			break;
		case 26:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/main_current");
			break;
		case 27:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_gfx_vcore");
			break;
		case 28:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_1v05");
			break;
		case 29:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_1v5");
			break;
		case 30:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/voltage_vin");
			break;
		case 31:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys2_fan_speed");
			break;
		case 32:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys3_fan_speed");
			break;
		case 33:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys2_cur_temp");
			break;
		case 34:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys2_min_temp");
			break;
		case 35:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys2_max_temp");
			break;
		case 36:
			sprintf(sysfile, "/sys/class/hwmon/hwmon2/device/sys2_startup_temp");
			break;
		case 37:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/power_cycles");
			break;
		case 38:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/bmc_flags");
			break;
		case 39:
			sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/bmc_status");
			break;
		default:
			status = EAPI_STATUS_UNSUPPORTED;
	}

	if (strlen(sysfile) == 0)
	{
		return EAPI_STATUS_SUCCESS;	
	}

	ret = read_sysfs_file(sysfile, res, sizeof(res));

	if (ret == 0)
		status = EAPI_STATUS_SUCCESS;
	printf("ret is: %d\n", ret);
	
	if (strlen(res) == 0 || ret == -1){
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (Id == 5)
	{
		*pValue = atoi(res);
		*pValue = encode_celcius(*pValue);
		return 0;
	}

	*pValue = atoi(res);


	return status;

}
static int get_regulator_voltage(int id, uint32_t *mVolts, char *buf, uint32_t size)
{
	struct dirent *de;  // Pointer for directory entry 
	char regulator[64];

	// opendir() returns a pointer of DIR type.  
	DIR *dr = opendir("/sys/class/regulator/"); 

	if (dr == NULL)  // opendir returns NULL if couldn't open directory 
	{ 
		printf("Could not open current directory" ); 
		return 0; 
	} 

	sprintf(regulator, "adl_bmc_reg%d", id);
	

	// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html 
	// for readdir() 
	while ((de = readdir(dr)) != NULL) {
		if(strncmp(de->d_name, "regulator", strlen("regulator")) == 0) {
			char sysfile_volt[288];
			char sysfile_desc[282];
			char value[256];
			
			sprintf(sysfile_volt, "/sys/class/regulator/%s/name", de->d_name);	
			if(read_sysfs_file(sysfile_volt, value, sizeof(value)) != 0) {
				continue;
			}
			
			if(strncmp(value, regulator, strlen(regulator)) != 0) {
				continue;
			}
			
			sprintf(sysfile_volt, "/sys/class/regulator/%s/microvolts", de->d_name);
			sprintf(sysfile_desc, "/sys/class/regulator/%s/name", de->d_name);
			if(read_sysfs_file(sysfile_volt, value, sizeof(value)) != 0) {
				return -1;
			}
			if(read_sysfs_file(sysfile_desc, buf, size) != 0) {
				return -1;
			}
			*mVolts = atoi(value);
			closedir(dr);     
			return 0;
		}
	}

	errno = EINVAL;
	closedir(dr);     
	return -1;
}

uint32_t EApiBoardGetVoltageMonitor(uint32_t id, uint32_t *mVolts, char *buf, uint32_t size)
{
	int ret;

	ret = get_regulator_voltage(id, mVolts, buf, size);
	
	return ret;

}

uint32_t EApiBoardGetErrorLog (uint32_t Pos, uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time, uint8_t *Status, signed char *CPUtemp, signed char *Boardtemp)
{
        char sysfile[128];
	int ret, i, j;
        unsigned char res[32];
	char buf[32];

	uint32_t status = EAPI_STATUS_SUCCESS;
	char *data[] = {"ErrorNumber", "Flags", "RestartEvent", "PowerCycle", "BootCount", "Time", "Status", "CPUTemp", "BoardTemp", NULL};
	char *value[9];

	char pBuffer[1024] = {0};

	memset(res, 0, sizeof(res));
	memset(buf, 0, sizeof(buf));
	/*store exception number to buf*/
	sprintf(buf, "%u", Pos);
	sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/error_log");

	ret = write_sysfs_file(sysfile, buf, sizeof(buf));
	if(ret < 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}	
	ret = read_sysfs_file(sysfile, pBuffer, sizeof(pBuffer));
	if(ret < 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}	

	char *test = pBuffer, *token;
	for(i = 0; (token = strtok(test, ": \n")) != NULL; i++)
	{
		for(j = 0; data[j] != NULL; j++)
		{
			if(strcmp(data[j], token) == 0)
			{
				token = strtok(NULL,": \n");
				value[j] = strdup(token);
			}
		}
		test = NULL;
	}

	*ErrorNumber = atoi(value[0]);
	strcpy((char *)Flags, value[1]);
	strcpy((char *)RestartEvent, value[2]);
	*PwrCycles = atoi(value[3]);
	*Bootcount = atoi(value[4]);
	*Time = atoi(value[5]);
	strcpy((char *)Status, value[6]);
	strcpy((char *)CPUtemp, value[7]);
	strcpy((char *)Boardtemp, value[8]);

	return status;
}

uint32_t EApiBoardGetCurPosErrorLog (uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time, uint8_t *Status, signed char *CPUtemp, signed char *Boardtemp)
{
	char sysfile[128];
	int ret, i, j;
        unsigned char res[32];

	uint32_t status = EAPI_STATUS_SUCCESS;
	char *data[] = {"ErrorNumber", "Flags", "RestartEvent", "PowerCycle", "BootCount", "Time", "Status", "CPUTemp", "BoardTemp", NULL};
	char *value[9];

	char pBuffer[1024] = {0};

	memset(res, 0, sizeof(res));
	/*store exception number to buf*/
	sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/cur_pos_error_log");

	ret = read_sysfs_file(sysfile, pBuffer, sizeof(pBuffer));
	if(ret < 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}	

	char *test = pBuffer, *token;
	for(i = 0; (token = strtok(test, ": \n")) != NULL; i++)
	{
		for(j = 0; data[j] != NULL; j++)
		{
			if(strcmp(data[j], token) == 0)
			{
				token = strtok(NULL,": \n");
				value[j] = strdup(token);
			}
		}
		test = NULL;
	}

	*ErrorNumber = atoi(value[0]);
	strcpy((char *)Flags, value[1]);
	strcpy((char *)RestartEvent, value[2]);
	*PwrCycles = atoi(value[3]);
	*Bootcount = atoi(value[4]);
	*Time = atoi(value[5]);
	strcpy((char *)Status, value[6]);
	strcpy((char *)CPUtemp, value[7]);
	strcpy((char *)Boardtemp, value[8]);

	return status;
}


uint32_t EApiBoardGetErrorNumDesc(uint32_t Pos, char *pBuf, uint32_t Size)
{
	char sysfile[128];
	int ret;
        unsigned char res[32];
	char buf[32];

	uint32_t status = EAPI_STATUS_SUCCESS;
	memset(res, 0, sizeof(res));
	memset(buf, 0, sizeof(buf));
	/*store exception number to buf*/
	sprintf(buf, "%u", Pos);
	sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/err_num_des");

	ret = write_sysfs_file(sysfile, buf, sizeof(buf));
	if(ret < 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}	
	ret = read_sysfs_file(sysfile, pBuf, Size);
	if(ret < 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}	
	
	if (pBuf[0] != 'e')
		return EAPI_STATUS_UNSUPPORTED;

        return status;
}


uint32_t EApiBoardGetExcepDesc(uint32_t exc_code, char *exc_desc, uint32_t size)
{
	char sysfile[128];
	int ret;
        unsigned char res[32];
	char buf[32];

	uint32_t status = EAPI_STATUS_SUCCESS;
	memset(res, 0, sizeof(res));
	memset(buf, 0, sizeof(buf));
	/*store exception number to buf*/
	sprintf(buf, "%u", exc_code);
	sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-boardinfo/information/exc_des");

	ret = write_sysfs_file(sysfile, buf, size);
	if(ret < 0) {
		return -1;
	}	

	ret = read_sysfs_file(sysfile, exc_desc, size);
	if(ret < 0) {
		return -1;
	}	

	return status;
}


