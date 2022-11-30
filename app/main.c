#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <errorcodes.h>
#include <eapi.h>
#include <ctype.h>
#include <conv.h>
#include <eapi.h>



char*			ExeName;
uint8_t	SetWatchdog, TriggerWatchdog, StopWatchdog, WatchDogCap,IsPwrUpWDogStart, IsPwrUpWDogStop;
uint8_t	StorageCap, StorageAreaRead, StorageAreaWrite, StorageAreaLock, StorageAreaUnLock,StorageHexWrite, StorgeHexRead;;
uint8_t	SmartFanTempSet, SmartFanTempGet, SmartFanTempSetSrc, SmartFanTempGetSrc, SmartFanPWMSet;
uint8_t	SmartFanModeGet, SmartFanModeSet, SmartFanPWMGet;
uint8_t	GetStringA, GetValue, GetVoltageMonitor;
uint8_t	VgaGetBacklightEnable, VgaSetBacklightEnable, VgaGetBacklightBrightness, VgaSetBacklightBrightness;
uint8_t	GPIOGetDirectionCaps, GPIOGetDirection, GPIOSetDirection, GPIOGetLevel, GPIOSetLevel;
uint8_t GetErrorLog, GetErrorNumberDescription, GetCurrentPosErrorLog, GetExceptionDescription;
uint8_t IsI2CCap, IsI2CProb, IsI2CWrRaw, IsI2CReRaw, IsI2CReXf, IsI2CWrXf, IsI2CSts;

uint8_t SetBiosSource, GetBiosSource,GetBiosStatus, srcdata;
struct {
	uint8_t BusID;
	uint8_t Address;
	uint32_t CmdType;
	uint32_t cmd;
	void* pBuffer;
	uint32_t nByteCnt;
}I2CFuncArgs;
unsigned int string_to_hex(char *string)
{
	int data = 0;
	if (string != NULL)
	{
		sscanf(string, "%x", &data);
	}
	return data;
}
static void errno_exit(const char *s) 
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

void ShowHelp(int condition)
{
	if (condition == 0)
	{
		printf("Usage:\n");
		printf("- Display this screen:\n");
		printf("	semautil /h\n\n");
	}
	if (condition == 1 || condition == 0)
	{
		printf("- Watch Dog:\n");
		printf("  1. semautil /w get_cap\n");
		printf("  2. semautil /w start [sec (0-65535)] \n");
		printf("  3. semautil /w trigger\n");
		printf("  4. semautil /w stop\n");
		printf("  5. semautil /w pwrup_enable [sec (60-65535)]\n");
		printf("     Note:\n	Start time will be different for the different platforms.\n	Please go to BIOS meun to check it\n");
		printf("  6. semautil /w pwrup_disable\n\n");
	}
	if (condition == 2 || condition == 0)
	{
		printf("- Storage:\n");
		printf("  1. semautil /s get_cap\n");
		printf("  2. semautil /s read [Region] [Address] [Length] \n");
		printf("  3. semautil /s write [Region] [Address] [string/value] [Length] \n");
		printf("  4. semautil /s hex_write [Region] [Address] [value] \n");
		printf("  5. semautil /s hex_read [Region] [Address] [Length] \n");
		printf("  6. semautil /s lock [Region]\n");
		printf("  7. semautil /s unlock [Region] [Permission] [passcode]\n\n");
		printf("     Region:\n");
		printf("     1.User\n");
		printf("     2.Secure\n\n");
		printf("     Permission:\n");
		printf("     1.Read only\n");
		printf("     2.Read/Write\n\n");
		printf("     Example: semautil /s write 1020 Aaaa 4\n          It will be written to 1020, 1021, 1022, 1023\n\n");
		//printf("\n     Note: Hexa decimal values are not valid\n     Note: Locking of ODM region will only make ODM is read-only.\n          lock function will not protect User region.\n          read and write function will not work on ODM region\n");
		printf("     Note : hex_write operation should be provided as below\n	Example: semautil /s hex_write 1 128 aa bb c d \n");

	}
	if (condition == 3 || condition == 0)
	{
		printf("- Smart FAN control:\n");
		printf("  1. semautil /f set_temp_points [FanID] [Level1] [Level2] [Level3] [Level4] \n");
		printf("  2. semautil /f set_pwm_points  [FanID] [PWMLevel1] [PWMLevel2] [PWMLevel3] [PWMlevel4] \n");
		printf("  3. semautil /f get_temp_points [FanID] \n");
		printf("  4. semautil /f get_pwm_points  [FanID] \n");
		printf("  5. semautil /f set_temp_source [FanID] [TempSrc]\n");
		printf("  6. semautil /f get_temp_source [FanID] \n");
		printf("  7. semautil /f get_mode [FanID] \n");
		printf("  8. semautil /f set_mode [FanID] [Mode]\n");
		printf("\n     FanID\n     0:CPU fan\n     1:System fan 1\n");
		printf("\n     Mode\n     0:Auto\n     1:Off\n     2:On\n     3:Soft\n");
		printf("\n     TempSrc\n     0-CPU sensor\n     1-Board sensor\n\n");

	}
	if (condition == 4 || condition == 0)
	{
		printf("- System monitor-Board Info:\n");
		printf("  1. semautil /i get_bd_info [EAPI ID] \n");
		printf("       1  : Board manufacturer name \n");
		printf("       2  : Board name \n");
		printf("       3  : Board serial number\n");
		printf("       4  : Board BIOS revision\n");
		printf("       5  : Board bootloader revision\n");
		printf("       6  : Board restart event\n");
		printf("       7  : Board HW revision \n");
		printf("       8  : Board application revision\n");
		printf("       9  : Board repair date\n");
		printf("       10 : Board manufacturer date\n");
		printf("       11 : Board MAC address\n");
		printf("       12 : Board 2nd HW revision number\n");
		printf("       13 : Board 2nd serial\n\n");
	}
	if (condition == 5 || condition == 0)
	{
		printf("- Voltage monitor:\n");
		printf("  1. semautil /v get_voltage [Channel (0-15)] \n\n");
	}

	if (condition == 6 || condition == 0)
	{
		printf("- Error log:\n");
		printf("  1. semautil /e get_error_log [Position(0-31)]\n");
		printf("  2. semautil /e get_cur_error_log\n");
		printf("  3. semautil /e get_bmc_error_code [Error Number]\n\n");
	}
	if (condition == 7 || condition == 0)
	{
		printf("- Exception Description :\n");
		printf("  1. semautil /x get_excep_desc\n\n");
	}
	if (condition == 8 || condition == 0)
	{
		printf("- GPIO:\n");
		printf("  1. semautil /g get_direction_cap [ID]\n");
		printf("  2. semautil /g get_direction     [GPIO Bit]\n");
		printf("  3. semautil /g set_direction     [GPIO Bit] [0 - Output or 1 - Input]\n");
		printf("  4. semautil /g get_level         [GPIO Bit]\n");
		printf("  5. semautil /g set_level         [GPIO Bit] [0 - Low or 1 - High]\n");
		printf("       GPIO set/write parameters:\n");
		printf("       GPIO Bit  1-16 \n");
		printf("       Note: GPIO access may not be available on all platforms\n\n");
	}
	if (condition == 9 || condition == 0)
	{
		printf("- Board values:\n");
		printf("  1. semautil /d  get_value [EAPI ID] \n");
		printf("       1	:  EAPI Specification Version\n");
		printf("       2	:  Boot Counter\n");
		printf("       3	:  Running time meter value\n");
		printf("       4	:  Vendor Specific Library Version\n");
		printf("       5	:  CPU Temperature\n");
		printf("       6	:  System Temperature\n");
		printf("       7	:  CPU Core Voltage\n");
		printf("       8	:  2.5V Voltage\n");
		printf("       9	:  3.3V Voltage\n");
		printf("       10	:  Battery Voltage\n");
		printf("       11	:  5V Voltage\n");
		printf("       12	:  5V Standby Voltage\n");
		printf("       13	:  12V Voltage\n");
		printf("       14	:  CPU Fan\n"); 
		printf("       15	:  System Fan 1\n");
		printf("       16	:  Get power uptime\n");
		printf("       17	:  Get restart event\n");
		printf("       18	:  Get BMC capabilities\n");
		printf("       19	:  Get extended BMC capabilities\n");
		printf("       20	:  Board Min Temperature\n");
		printf("       21	:  Board Max Temperature\n");
		printf("       22	:  Board Startup Temperature\n");
		printf("       23	:  CPU Min Temperature\n");
		printf("       24	:  CPU Max Temperature\n");
		printf("       25	:  CPU startup Temperature\n");
		printf("       26	:  Get main power current\n");
		printf("       27	:  GFX Voltage\n");
		printf("       28	:  1.05 Voltage\n");
		printf("       29	:  1.5 Voltage\n");
		printf("       30	:  Vin Voltage\n");
		printf("       31	:  System Fan 2\n");
		printf("       32	:  System Fan 3\n");
		printf("       33	:  Board 2nd Current Temperature\n");
		printf("       34	:  Board 2nd Min Temperature\n");
		printf("       35	:  Board 2nd Max Temperature\n");
		printf("       36	:  Board 2nd Startup Temperature\n");
		printf("       37	:  Get Board power cycle counter\n");
		printf("       38	:  Get Board BMC Flag\n");
		printf("       39	:  Get Board BMC Status\n");
		printf("       40	:  IO Current\n\n");
	}
	if (condition == 10 || condition == 0)
	{
		printf("- LVDS Backlight control:\n");
		printf("  1. semautil /b  set_bkl_value   [ID] [Level (0-255)]\n");
		printf("  2. semautil /b  set_bkl_enable  [ID] [0-Disable or 1-Enable]\n");
		printf("  3. semautil /b  get_bkl_value   [ID]\n");
		printf("  4. semautil /b  get_bkl_enable  [ID]\n");
		printf("       [ID]       LCD\n");
		printf("        0    EAPI_ID_BACKLIGHT_1\n");
		printf("        1    EAPI_ID_BACKLIGHT_2\n");
		printf("        2    EAPI_ID_BACKLIGHT_3\n\n");
	}

	if (condition == 11 || condition == 0)
	{
		printf("\n- Generic I2C Read/Write:\n");
		printf("  1. semautil /i2c  bus_cap\n");
		printf("  2. semautil /i2c  probe_device [bus id]\n");
		printf("  3. semautil /i2c  write_raw	 [bus id] [address] [length] byte0 byte1 byte2 ...\n");
		printf("  3. semautil /i2c  read_raw	 [bus id] [address] [length]\n");
		printf("  4. semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [length]\n");
		printf("  5. semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [cmd] [length] byte0 byte1 byte2 ...\n");
		printf("  6. semautil /i2c  get_status\n");

		printf("  [Bus Id]:\n");
		printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
		printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
		printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
		printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
		
		printf("    4\tEAPI_ID_I2C_LVDS_2\t\tLVDS \\ EDP 2 Interface\n");

		printf("  [Command type]:\n");
		printf("    ID\tENCODED CMD ID\t\tDescription\n");
		printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
		printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
		printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n\n");
	}

	 if(condition == 12 || condition == 0)
        {
                printf("\n- Get BIOS Source:\n");
                printf("  1. semautil /src  get_src\n");
                printf("  2. semautil /src  set_src value[0-3]\n");
		printf("  3. semautil /src get_bios_status\n");
                printf("  \n	Value   :\n");
                printf("	 0      -   By hardware configuration of currently selected BIOS\n");
                printf("	 1      -   Switch to Fail-Safe BIOS\n");
                printf("	 2      -   Switch to External BIOS (SPI0 on carrier)\n");
                printf("	 3      -   Switch to Internal BIOS (SPI0 on module)\n");

		printf("\n");
		printf("  BIOS select status information.\n");
		printf("	Bit2 Bit1 Bit0\n");
                printf("	 0    0    0  -   Module SPI0/ Carrier SPI1 (Standard BIOS)\n");
                printf("	 0    0    1  -   Carrier SPI0/ Module SPI1 (Fail - Safe BIOS)\n");
                printf("	 0    1    0  -   Unknown\n");
                printf("	 0    1    1  -   Module SPI0/Module SPI1 (Standard BIOS)\n");
		printf("	 1    0    0  -   Unknown\n");
                printf("	 1    0    1  -   Switch to Fail-Safe BIOS\n");
                printf("	 1    1    0  -   Switch to External BIOS\n");
                printf("	 1    1    1  -   Switch to Internal BIOS \n\n");
		printf("  If Bit 2 is OFF : PICMG BIOS selected\n");
		printf("  If Bit 2 is ON : Dual BIOS selected\n");
	
        }
	 
}
int DispatchCMDToSEMA(int argc,char *argv[])
{
	int ret  = 0;
	/* Board information*/
	uint32_t Id, Size,region,permission, Value = 0;
	char *passcode;
	char BoardInfo[64];
	char ExcepDesc[1024];
	memset(BoardInfo, 0, sizeof(BoardInfo));			
	memset(ExcepDesc, 0, sizeof(ExcepDesc));
	uint8_t ExceptionCode;
	uint32_t Pos, ErrorNumber = 0;
	uint8_t  Flags[20], RestartEvent[20], BiosSel;
	uint32_t PwrCycles, Bootcount, Time, TotalOnTime;
	uint8_t Status[20] = {0};
	signed char CPUtemp[20] = {0}, Boardtemp[20] = {0};
	/*Voltage Monitor*/
	uint32_t Vid, Voltage;
	char Vmbuf[32];
	memset(Vmbuf, 0, sizeof(Vmbuf));				
	/* Backlight */
	uint32_t bid, enable;
	uint32_t brightness;
	/* Watchdog */
	uint32_t MaxDelay, MaxEventTimeout, MaxResetTimeout, delay, EventTimeout, ResetTimeout;
	/* Fan */
	int fid = 0, Level1, Level2, Level3, Level4, Tempsrc, fan_mode, sts;
	/* Storage */
	uint32_t Offset, BufLen, ByteCnt;
	char *Buffer;
	unsigned char memcap[4096];
	uint32_t Storagesize = 0, BlockLength = 0;

	// Library Initializing
	ret = EApiLibInitialize();
	if (ret){
		if (ret == -1){
			printf("Initialization Failed\n");
			exit(EXIT_FAILURE);
		}
		else
			errno_exit("Initialization");
	}
	if (VgaGetBacklightEnable)
	{
		if (argc != 4) {
			printf("Wrong arguments \n");
			exit(-1);
		}					
		bid = atoi(argv[3]);
		ret = EApiVgaGetBacklightEnable(bid, &enable);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiVgaGetBackLightEnable");
		} 
		if(enable)
			printf("Backlight ON\n");
		else
			printf("Backlight OFF\n");
	}
	if (VgaSetBacklightEnable)
	{
		if (argc != 5) {
			printf("Wrong arguments \n");
			exit(-1);
		}
		bid = atoi(argv[3]);
		enable = atoi(argv[4]);
		ret = EApiVgaSetBacklightEnable(bid, enable);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiVgaSetBackLightEnable");
		}
		if(enable)
			printf("Backlight ON\n");
		else
			printf("Backlight OFF\n");
	}
	if (VgaGetBacklightBrightness)
	{
		if (argc != 4) {
			printf("Wrong arguments \n");
			exit(-1);
		}
		bid = atoi(argv[3]);
		ret = EApiVgaGetBacklightBrightness(bid, &brightness);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiVgaGetBackLightBrightness");
		}
		printf("Current Backlight Brightness is  %u\n", brightness);
	}
	if(VgaSetBacklightBrightness)
	{
		if (argc != 5) {
			printf("Wrong arguments \n");
			exit(-1);
		}
		bid = atoi(argv[3]);
		brightness = atoi(argv[4]);
		ret = EApiVgaSetBacklightBrightness(bid, brightness);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiVgaSetBackLightBrightness");
		}
		printf("Current Backlight Brightness is set to %u\n", brightness);
	}
	if (GetValue)
	{
		if (argc != 4) {
			printf("Wrong arguments GetValue\n");
			exit(-1);
		}
		Id = atoi(argv[3]);
		ret = EApiBoardGetValue(Id, &Value);
		if (ret){
			printf("Get EApi information failed\n");
			errno_exit("EApiBoardGetValue");
		}
		printf("%d\n",Value);
	}
	if (GetStringA)
	{
		if (argc != 4) {
			printf("Wrong arguments GetStringA\n");
			exit(-1);
		}
		Id = atoi(argv[3]);
		Size = sizeof(BoardInfo);
		ret = EApiBoardGetStringA(Id, BoardInfo, &Size);
		if (ret) {
			if(ret==EAPI_STATUS_UNSUPPORTED)
				printf("Board does not support this capability\n");
			else
				printf("Get EApi information failed\n");
			errno_exit("EApiBoardGetStringA");
		}
		printf("\n%s\n", BoardInfo);
	}
	if (WatchDogCap)
	{
		ret = EApiWDogGetCap(&MaxDelay, &MaxEventTimeout, &MaxResetTimeout);
		if (ret)
		{
			printf("Get EAPI information failed\n");
			errno_exit("EApiWDogGetCap");
		}
		printf("MaxEventTimeout : %u\nMaxDelay : %u\nMaxResetValue : %u\n",MaxEventTimeout,MaxDelay,MaxResetTimeout);
	}
	if (SetWatchdog)
	{
		if (argc != 4) {
			printf("Wrong arguments SetWatchdog\n");
			exit(-1);
		}
		delay = 0;
		EventTimeout = 0;
		ResetTimeout = atoi(argv[3]);
		ret = EApiWDogStart(delay, EventTimeout, ResetTimeout);
		if (ret == EAPI_STATUS_RUNNING)
		{
			errno = EALREADY;
			errno_exit("EApiWDogStart");
		}

		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiWDogStart");
		}
		printf("Run-time Watchdog Started with : %u seconds \n",ResetTimeout);
	}
	if (TriggerWatchdog)				
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		ret = EApiWDogTrigger();
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiWDogTrigger");
		}
		printf("Watchdog is triggered to previous number of seconds again\n");
	}
	if (StopWatchdog)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		ret = EApiWDogStop();
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiWDogStop");
		}
		printf("Watchdog Stoped Successfully\n");
	}

	if (IsPwrUpWDogStart)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		ResetTimeout = atoi(argv[3]);
		ret = EApiPwrUpWDogStart(ResetTimeout);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiPwrUpWDogStart");
		}
		printf("PowerUp Watchdog Started with : %u seconds \n",ResetTimeout);
	}

	if (IsPwrUpWDogStop)				 
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		ret = EApiPwrUpWDogStop();
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiPwrUpWDogStop");
		}
		printf("Watchdog is disabled  successfully\n");
	}


	if (SmartFanTempSet)
	{
		if (argc != 8) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		Level1 = atoi(argv[4]);
		Level2 = atoi(argv[5]);
		Level3 = atoi(argv[6]);
		Level4 = atoi(argv[7]);

		ret = EApiSmartFanSetTempSetpoints(fid, Level1, Level2, Level3, Level4);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiSmartFanSetTempSetpoints");
		}
		printf("Temperature levels set successfully\n");
	}

	if (SmartFanTempGet)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		ret = EApiSmartFanGetTempSetpoints(fid, &Level1, &Level2, &Level3, &Level4);
		if (ret) {
			printf("Get EAPI information failed\n");
			errno_exit("EApiSmartFanGetTempSetpoints");
		}
		if(fid)
			printf("Fan ID: %d (System fan)\nLevel1: %d\nLevel2: %d\nLevel3: %d\nLevel4: %d\n", fid, Level1, Level2, Level3, Level4);
		else
			printf("Fan ID: %d (CPU fan)\nLevel1: %d\nLevel2: %d\nLevel3: %d\nLevel4: %d\n", fid, Level1, Level2, Level3, Level4);

	}

	if (SmartFanPWMSet)
	{
		if (argc != 8) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		Level1 = atoi(argv[4]);
		Level2 = atoi(argv[5]);
		Level3 = atoi(argv[6]);
		Level4 = atoi(argv[7]);

		ret = EApiSmartFanSetPWMSetpoints(fid, Level1, Level2, Level3, Level4);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiSmartFanSetPWMSetpoints");
		}
		printf("PWM levels set successfully\n");
	}

	if (SmartFanPWMGet)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		ret = EApiSmartFanGetPWMSetpoints(fid, &Level1, &Level2, &Level3, &Level4);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiSmartFanGetPWMSetpoints");
		}
		if(fid)
			printf("Fan ID: %d (System fan)\nLevel1: %d\nLevel2: %d\nLevel3: %d\nLevel4: %d\n", fid, Level1, Level2, Level3, Level4);
		else
			printf("Fan ID: %d (CPU fan)\nLevel1: %d\nLevel2: %d\nLevel3: %d\nLevel4: %d\n", fid, Level1, Level2, Level3, Level4);
	}

	if (SmartFanModeSet)
	{
		if (argc != 5) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		fan_mode = atoi(argv[4]);
		ret = EApiSmartFanSetMode(fid, fan_mode);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanSetMode");
		}
		printf("FAN mode set successfully\n");
	}


	if (SmartFanModeGet)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		ret = EApiSmartFanGetMode(fid, &fan_mode);
		if (ret) {
			if(ret==EAPI_STATUS_UNSUPPORTED)
				printf("Board does not support this capability\n");
			else
				printf("Get EApi information failed\n");
			errno_exit("EApiSmartFanGetMode");
		}
		if(fid)
		{

			if(fan_mode==0)
				printf("Fan id: %d (System fan) \nFan Mode: %d(Auto)\n", fid, fan_mode);
			else if(fan_mode==1)
				printf("Fan id: %d (System fan) \nFan Mode: %d(Off)\n", fid, fan_mode);
			else if(fan_mode==2)
				printf("Fan id: %d (System fan) \nFan Mode: %d(On)\n", fid, fan_mode);
			else
				printf("Fan id: %d (System fan) \nFan Mode: %d(Soft)\n", fid, fan_mode);
		}
		else
		{
			if(fan_mode==0)
				printf("Fan id: %d (CPU fan) \nFan Mode: %d(Auto)\n", fid, fan_mode);
			else if(fan_mode==1)
				printf("Fan id: %d (CPU fan) \nFan Mode: %d(Off)\n", fid, fan_mode);
			else if(fan_mode==2)
				printf("Fan id: %d (CPU fan) \nFan Mode: %d(On)\n", fid, fan_mode);
			else 
				printf("Fan id: %d (CPU fan) \nFan Mode: %d(Soft)\n", fid, fan_mode);

		}
	}

	if (SmartFanTempSetSrc)
	{
		if (argc != 5) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		Tempsrc = atoi(argv[4]);
		ret = EApiSmartFanSetTempSrc(fid, Tempsrc);
		if (ret) {
			if(ret==EAPI_STATUS_UNSUPPORTED)
				printf("Board does not support this capability\n");
			else
				printf("Get EApi information failed\n");
			errno_exit("EApiSmartFanSetTempSrc");
		}
		if(Tempsrc)
			printf("Temperature source is set to Baseboard sensor\n");
		else
			printf("Temperature source is set to CPU sensor\n");
	}


	if (SmartFanTempGetSrc)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		fid = atoi(argv[3]);
		ret = EApiSmartFanGetTempSrc(fid, &Tempsrc);
		if (ret) {
			if(ret==EAPI_STATUS_UNSUPPORTED)
				printf("Board does not support this capability\n");
			else
				printf("Get EApi information failed\n");
			errno_exit("EApiSmartFanGetTempSrc");
		}
		if(Tempsrc)
			printf("Current Temperature source is Baseboard sensor\n");
		else
			printf("Current Temperature source is CPU sensor\n");
	}


	// Storage Functions Start

	if (StorageCap)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}

		Id = EAPI_ID_STORAGE_STD;
		ret = EApiStorageCap(Id, &Storagesize, &BlockLength);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiStorageCap");
		}
		printf("Storage Size: %u\nBlock Size: %u\n", Storagesize, BlockLength);
	}


	if (StorageAreaRead)
	{
		int cnt;
		if (argc != 6) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Id = EAPI_ID_STORAGE_STD;
		memset(memcap, 0, sizeof(memcap));
		region= atoi(argv[3]);
		Offset = atoi(argv[4]);
		ByteCnt = atoi(argv[5]);
		BufLen = sizeof(memcap);
		ret = EApiStorageAreaRead(Id,region, Offset, memcap, BufLen, ByteCnt);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiStorageAreaRead");
		}
		for (cnt =0; cnt<BufLen; cnt++)
		{
			if(isprint(memcap[cnt]) == 0)
			{
				memcap[cnt] = 0;
				break;
			}
		}

		printf("Read Buffer: %s\n", memcap);
	}


	if (StorageAreaWrite)
	{
		if (argc != 7) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Id = EAPI_ID_STORAGE_STD;
		region = atoi(argv[3]);
		Offset = atoi(argv[4]);
		Buffer = argv[5];
		ByteCnt = atoi(argv[6]);
		ret = EApiStorageAreaWrite(Id,region, Offset, Buffer, ByteCnt);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiStorageAreaWrite");
		}
		printf("Data Written Successfully\n");
	}
	
	if(StorageHexWrite)
	{
		if(argc != 6){
			if(argc < 6)
			{
				printf("Wrong arguments\n");
				exit(-1);
			}
		}
		char hex_buf[2048];
		int i,j=0;
		
		for(i=5;i<argc;i++)
                {
			if(strlen(argv[i])==1)
			{
			       hex_buf[j] = '0';
			       j++;
		       	       hex_buf[j] = argv[i][0];	       
			}
			else
			{
                        	strcpy(hex_buf + j,argv[i]);
			}
                        j = strlen (argv[i]) + j;
		}
		Id = EAPI_ID_STORAGE_STD;
                region = atoi(argv[3]);
                Offset = atoi(argv[4]);
		Buffer = hex_buf;
                ByteCnt = strlen(Buffer);
		ret = EApiStorageHexWrite(Id,region, Offset, Buffer, ByteCnt/2);
                if (ret) {
                        printf("Get EApi information failed\n");
                        errno_exit("EApiStorageHexWrite");
                }
                printf("%d Bytes Written Successfully\n",ByteCnt/2);

	}
	
	if(StorgeHexRead)
	{
                if (argc != 6) {
                        printf("Wrong arguments\n");
                        exit(-1);
                }
                Id = EAPI_ID_STORAGE_STD;
                memset(memcap, 0, sizeof(memcap));
                region= atoi(argv[3]);
                Offset = atoi(argv[4]);
                ByteCnt = atoi(argv[5]);
                BufLen = sizeof(memcap);

                ret = EApiStorageHexRead(Id,region, Offset, memcap, BufLen, ByteCnt);
                if (ret) {
                        printf("Get EApi information failed\n");
                        errno_exit("EApiStorageHexRead");
                }
		printf("Read Buffer : ");
		for(int i=0;i<ByteCnt;i++)
		{
                	printf("0x%02X ", memcap[i]);
		}
		printf("\n");

	}
	if(StorageAreaLock)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		region = atoi(argv[3]);
		Id = 1;
		ret = EApiStorageLock(Id,region);
		if (!ret) {
			if(region==2)
			printf("Secured region locked successfully\n");
			else if(region==3)
			printf("ODM region locked successfully\n");
		}
		else
		{
			printf("Get EApi information failed\n");
			errno_exit("EApiStorageAreaLock");
		}
	}

	if(StorageAreaUnLock)
	{
		if (argc != 6) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Id = 1;
		region = atoi(argv[3]);
		permission=atoi(argv[4]);
		passcode=argv[5];
		ret = EApiStorageUnLock(Id,region,permission, passcode);
		if (!ret) {
			if(region==2)
			printf("Secure region is unLocked successfully\n");
			else if(region==3)
			printf("ODM region is unLocked successfully\n");
		}
		else
		{
			printf("Get EApi information failed\n");
			errno_exit("EApiStorageAreaUnLock");
		}
	}



	if (GPIOGetDirectionCaps)
	{
		if (argc != 4){
			printf("Wrong arguments\n");
			exit(-1);
		}
		uint32_t input , output;
		Id = atoi(argv[3]);
		if (Id > 12 || Id == 0)
                {
                        printf("GPIO value should be 1-8 or 1-12\n");
                        printf("Note: GPIO access may not be available on all platforms\n\n");
                        return -1;
                }

		ret = EApiGPIOGetDirectionCaps(Id, &input, &output);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiGPIOGetDirectionCaps");
		}
		if(input!=0 && output!=0)
			printf("Input/Output supported for GPIO %d\n",Id);
		else if(input!=0)
			printf("Input supported for GPIO %d\n",Id);
		else
			printf("Output supported for GPIO %d\n",Id);
	}


	if (GPIOGetDirection)
	{
		uint32_t bitmask=0, dir;
		if(argc!=4)
		{
			printf("Wrong arguments\n");
			exit(-1);
		}
		bitmask = atoi(argv[3]); //bitmask --> user provided gpio number. 
		if (bitmask > 12 || bitmask == 0)
		{
			printf("GPIO value should be 1-8 or 1-12\n");
			printf("Note: GPIO access may not be available on all platforms\n\n");
			return -1;
		}
		bitmask--;
		bitmask = (1 << bitmask);
		ret = EApiGPIOGetDirection(0, bitmask, &dir);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiGPIOGetDirection");
		}
		if(dir)
			printf("Direction : Input\n");
		else 
			printf("Direction : Output\n");
	}


	if (GPIOSetDirection)
	{
		uint32_t bitmask=0, dir=0;
		if(argc!=5)
		{
			printf("Wrong arguments\n");
			exit(-1);
		}
		bitmask = atoi(argv[3]);
		dir = atoi(argv[4]);
		if (bitmask > 12 || bitmask == 0)
		{
			printf("GPIO value should be 1-8 or 1-12\n");
			printf("Note: GPIO access may not be available on all platforms\n\n");
			return -1;
		}
		if ((dir < 0) || (dir > 1))
		{
			printf("invalid gpio direction value\n");
			return -1;
		}
		bitmask--;
		bitmask = 1 << bitmask;
		ret = EApiGPIOSetDirection(0, bitmask, dir);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiGPIOSetDirection");
		}
		printf("Direction updated successfully\n");
	}

	if (GPIOGetLevel)
	{
		if(argc!=4)
		{
			printf("Wrong arguments\n");
			exit(-1);
		}
		uint32_t bitmask=0, val=0;

		bitmask = atoi(argv[3]);
		if (bitmask > 12 || bitmask == 0)
		{
			printf("GPIO pin number should be 1-8 or 1-12\n");
			printf("Note: GPIO access may not be available on all platforms\n\n");
			return -1;
		}
		bitmask--;
		bitmask = (1 << bitmask);

		ret = EApiGPIOGetLevel(0, bitmask, &val);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiGPIOGetLevel");
		}
		if(val)
			printf("Level: High\n");
		else 
			printf("Level: Low\n");
	}


	if (GPIOSetLevel)
	{
		uint32_t bitmask=0, val=0;
		if(argc!=5)
		{
			printf("Wrong arguments\n");
			exit(-1);
		}
		bitmask = atoi(argv[3]);
		val = atoi(argv[4]);
		if (bitmask > 12 || bitmask == 0)
		{
			printf("GPIO pin number should be 1-8 or 1-12\n");
			printf("Note: GPIO access may not be available on all platforms\n\n");
			return -1;
		}
		if ((val < 0) || (val > 1))
		{
			printf("invalid gpio value\n");
			return -1;
		}
		bitmask--;
		val = val << bitmask;
		bitmask = 1 << bitmask;
		ret = EApiGPIOSetLevel(0, bitmask, val);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiGPIOSetLevel");
		}
		printf("GPIO Level updated successfully\n");
	}


	if (GetErrorLog)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		Pos = atoi(argv[3]);
		ret = EApiBoardGetErrorLog (Pos, &ErrorNumber, Flags, RestartEvent, &PwrCycles, &Bootcount, &Time, Status, CPUtemp,Boardtemp, &TotalOnTime, &BiosSel);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiBoardGetErrororLog");
		}
		printf("ErrorNumber:    %u\nFlags:          %s\nRestartEvent:   %s\nPowerCycle:     %u\nBootCount:      %u\n",ErrorNumber, Flags, RestartEvent, PwrCycles, Bootcount);
		printf("Time:           %u\nTotalOnTime:	%u\nBios Sel:	%x\nStatus:         %s\nCPUTemp:        %s\nBoardTemp:      %s\n",Time, TotalOnTime, BiosSel, Status, CPUtemp, Boardtemp);
	}


	if (GetErrorNumberDescription)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		Pos = atoi(argv[3]);
		ret = EApiBoardGetErrorNumDesc(Pos, ExcepDesc, Size);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiBoardGetErrorNumDescLog");
		}
		printf("\n%s\n", ExcepDesc);
	}

	if (GetCurrentPosErrorLog)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		ret = EApiBoardGetCurPosErrorLog (&ErrorNumber, Flags, RestartEvent, &PwrCycles, &Bootcount, &Time, Status, CPUtemp, Boardtemp, &TotalOnTime, &BiosSel);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiBoardGetCurPosErrorLog");
		}
		printf("ErrorNumber:    %u\nFlags:          %s\nRestartEvent:   %s\nPowerCycle:     %u\nBootCount:      %u\n",ErrorNumber, Flags, RestartEvent, PwrCycles, Bootcount);
		printf("Time:           %u\nTotalOnTime:	%u\nBIOS Sel:	%x\nStatus:         %s\nCPUTemp:        %s\nBoardTemp:      %s\n",Time, TotalOnTime, BiosSel, Status, CPUtemp, Boardtemp);
	}

	if (GetExceptionDescription)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		for(ExceptionCode =0;ExceptionCode <= 24; ExceptionCode++)
		{
			ret = EApiBoardGetExcepDesc(ExceptionCode, ExcepDesc, Size);
			if (ret) {
				printf("get eapi information failed\n");
				errno_exit("EApiBoardGetExcepDesc");
			}
			if(ExcepDesc[0] != 0)
			{
				if(strncmp(ExcepDesc,"INVALID",strlen("INVALID"))== 0)
					break;
				else
					printf("%2u -> %s\n", ExceptionCode, ExcepDesc);

			}
		}
	}

	if (GetVoltageMonitor)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Vid = atoi(argv[3]);
		Size = sizeof(Vmbuf);
		ret = EApiBoardGetVoltageMonitor(Vid, &Voltage, Vmbuf, Size);
		if (ret) {
			printf("Get EApi information failed\n");
			errno_exit("EApiBoardGetVoltageMonitor");
		}
		printf("Voltage: %u mv\nDescription: %s\n",Voltage, Vmbuf);
	}

	char* list[] = { "EAPI_ID_I2C_EXTERNAL_1", "EAPI_ID_I2C_EXTERNAL_2", "EAPI_ID_I2C_EXTERNAL_3", "EAPI_ID_I2C_EXTERNAL_4" };


	if (IsI2CCap)
	{
		int i;
		uint32_t pMaxBlkLen;
		printf("\n");
		for (i = 0; i < 4; i++)
		{
			if ((sts = EApiI2CGetBusCap(i, &pMaxBlkLen)) == 0)
			{
				printf("%-28s is supported. maximum read size is %d bytes, and maximum write size is %d bytes.\n", list[i], pMaxBlkLen, 29);
			}
			else
			{
				printf("%-28s is not supported\n", list[i]);
			}
		}
		sts = 0;
		printf("\n");
		return sts;
	}

	if (IsI2CProb)
	{
		if(argc!=4)
		{
			printf("Wrong Argumnets\n");
			exit(-1);
		}
		volatile int i, once = 1, j = 1, status;
		uint32_t MaxBlkSize;
		if ((status = EApiI2CGetBusCap(I2CFuncArgs.BusID - 1, &MaxBlkSize)) != 0) 
		{
			if(status == EAPI_STATUS_UNSUPPORTED)
			{
				printf("\n%s is not Exist\n\n", list[I2CFuncArgs.BusID - 1]);
			}
			return 0;
		}
		for (i = 1; i < 127; i++)
		{
			if ((sts = EApiI2CProbeDevice(I2CFuncArgs.BusID - 1, i << 1)) == 0)
			{
				if (once == 1)
				{
					printf("\nSlave list in %s bus:\n", list[I2CFuncArgs.BusID - 1]);
					once = 0;
				}
				printf("%02d. %x\n",j++, i);
			}
			fflush(stdout);
			fflush(stdin);
			fflush(stderr);
		}
		if (once == 0)
		{
			printf("\n");
			return 0;
		}
		else
		{
			printf("\nNo slave devices found on %s bus\n\n", list[I2CFuncArgs.BusID - 1]);
			return 0;
		}
	}

	if (IsI2CSts)
	{
		uint8_t status;
		if ((sts = EApiI2CGetBusSts(I2CFuncArgs.BusID, &status)) == 0)
		{
			printf("\nThe bus status is %02x\n\n", status);
		}

		if(sts == -3)
		{
			printf("Get EApi information failed\n");
		}

		return 0;
	}

	if (IsI2CReXf)
	{
		   if ((sts = EApiI2CProbeDevice(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address)) != 0)
		   {
		   printf("\nSlave device not found with this address %02x\n\n", I2CFuncArgs.Address >> 1);
		   sts = 0;
		   }
		   else if ((sts = EApiI2CReadTransfer(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address, I2CFuncArgs.cmd, I2CFuncArgs.pBuffer, I2CFuncArgs.nByteCnt, I2CFuncArgs.nByteCnt)) == 0)
		{
			unsigned int i;
			printf("Read data:\n\n");
			for (i = 0; i < I2CFuncArgs.nByteCnt; i++)
			{
				printf("%02d : %02x\n", i, ((unsigned char*)(I2CFuncArgs.pBuffer))[i]);
			}
			printf("\n");
		}
		else
			printf("Get EApi information failed\n");
		return sts;
	}

	if (IsI2CWrXf)
	{
		   if ((sts = EApiI2CProbeDevice(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address)) != 0)
		   {
		   printf("\nSlave device not found with this address %02x\n\n", I2CFuncArgs.Address >> 1);
		   sts = 0;
		   }

		   else if ((sts = EApiI2CWriteTransfer(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address, I2CFuncArgs.cmd, I2CFuncArgs.pBuffer, I2CFuncArgs.nByteCnt)) == 0)
		{
			printf("\nThe I2C Write Transfer command is completed\n\n");
		}
		else
			printf("Get EAoi information failed\n");
		return sts;
	}

	if (IsI2CReRaw)
	{
		if ((sts = EApiI2CProbeDevice(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address)) != 0)
		  {
		  printf("\nSlave device not found with this address %02x\n\n", I2CFuncArgs.Address >> 1);
		  sts = 0;
		  }
		if((sts = EApiI2CWriteReadRaw(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address, NULL, 0, I2CFuncArgs.pBuffer, I2CFuncArgs.nByteCnt + 1, I2CFuncArgs.nByteCnt + 1)) == 0)
		{
			unsigned int i;
			printf("Read data:\n\n");
			for (i = 0; i < I2CFuncArgs.nByteCnt; i++)
			{
				printf("%02d : %02x\n", i, ((unsigned char*)(I2CFuncArgs.pBuffer))[i]);
			}
			printf("\n");
		}
		else
			printf("Get EApi information failed\n");
		return sts;
	}

	if (IsI2CWrRaw)
	{
		if ((sts = EApiI2CProbeDevice(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address)) != 0)
		  {
		  printf("\nSlave device not found with this address %02x\n\n", I2CFuncArgs.Address >> 1);
		  sts = 0;
		  }
		  else if ((sts = EApiI2CWriteReadRaw(I2CFuncArgs.BusID - 1, I2CFuncArgs.Address, I2CFuncArgs.pBuffer, I2CFuncArgs.nByteCnt + 1, NULL, 0, 0)) == 0)
		{
			printf("\nData Written Successfully\n\n");
		}
		else
			printf("Get EApi information failed\n");
		return sts;
	}

	 if(SetBiosSource)
        {
                 if((sts= EApiSetBiosSource(srcdata))==0)
                 {
                        printf("Bios boot source mode set successfully\n ");
                 }
                else
                        printf("Get EApi information failed\n");
                 return sts;
        }

        if(GetBiosSource)
        {
                if ((sts = EApiGetBiosSource(&srcdata)) ==0)
                {

                       srcdata -= '0';
                        switch (srcdata)
                        {
                        case 0:
                                printf(" 0  - By hardware configuration of currently selected BIOS\n");
                                break;
                        case 1:
                                printf(" 1  - Switch to Fail - Safe BIOS\n");
                                break;
                        case 2:
                                printf(" 2  - Switch to External BIOS(SPI0 on carrier)\n");
                                break;
                        case 3:
                                printf(" 3  - Switch to Internal BIOS(SPI0 on module)\n");
                                break;
                        }
                }
                else
                        printf("Get EApi information failed\n");
                return sts;
        }
	
	if(GetBiosStatus)
	{
		if((sts= EApiGetBiosStatus(&srcdata))==0)
		{
			srcdata -= '0';
			switch (srcdata)
                        {
                        case 0:
               			 printf("PICMG BIOS selected\n");
                                 printf(" 0    0    0  -  Module SPI0/ Carrier SPI1 (Standard BIOS)\n");
                                 break;
                        case 1:
				 printf("PICMG BIOS selected\n");
                		 printf(" 0    0    1  -   Carrier SPI0/ Module SPI1 (Fail - Safe BIOS)\n");
                                 break;
                        case 2:
				 printf("PICMG BIOS selected\n");
                		 printf(" 0    1    0  -   Unknown\n");
                                 break;
                        case 3:
				 printf("PICMG BIOS selected\n");
                		 printf(" 0    1    1  -   Module SPI0/Module SPI1 (Standard BIOS)\n");
                                 break;
                        case 4:
                		printf("Dual BIOS selected\n");
                		printf(" 1    0    0  -   Unknown\n");
                                break;
                        case 5:
                		printf("Dual BIOS selected\n");
               		 	printf(" 1    0    1  -   Switch to Fail-Safe BIOS\n");
                                break;
                        case 6:
                		printf("Dual BIOS selected\n");
                		printf(" 1    1    0  -   Switch to External BIOS\n");
				break;
                        case 7:
                		printf("Dual BIOS selected\n");
                		printf(" 1    1    1  -   Switch to Internal BIOS \n");
                                break;
                        }
		}
                else
                        printf("Get EApi information failed\n");
                return sts;
	}
	return 0;
}

signed int ParseArgs(int argc, char* argv[])
{
	int help_condition = 0;
	int eRet = 1;
	if (argc == 1)
	{
		ShowHelp(help_condition);
		return -1;
	}
	if (strcasecmp(argv[1], "/h") == 0)
	{
		ShowHelp(help_condition);
		return -1;
	}
	/* Watch Dog*/
	else if (strcasecmp(argv[1], "/w") == 0)
	{
		if (argc == 3 && (strcasecmp(argv[2], "get_cap") == 0))
		{
			WatchDogCap = TRUE;
		}
		else if (argc == 4 && strcasecmp(argv[2], "start") == 0)
		{
			SetWatchdog = TRUE;
		}
		else if (argc == 3 && (strcasecmp(argv[2], "trigger") == 0))
		{
			TriggerWatchdog = TRUE;
		}
		else if (argc == 3 && (strcasecmp(argv[2], "stop") == 0))
		{
			StopWatchdog = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "pwrup_enable") == 0))
		{
			IsPwrUpWDogStart = TRUE;
		}
		else if (argc == 3 && (strcasecmp(argv[2], "pwrup_disable") == 0))
		{
			IsPwrUpWDogStop = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 1;
		}
	}
	else if (strcasecmp(argv[1], "/s") == 0)
	{
		if (argc == 3 && (strcasecmp(argv[2], "get_cap") == 0))
		{
			StorageCap = TRUE;
		}
		else if (argc == 6 && (strcasecmp(argv[2], "read") == 0))
		{
			StorageAreaRead = TRUE;
		}
		else if (argc == 7 && (strcasecmp(argv[2], "write") == 0))
		{
			StorageAreaWrite = TRUE;
		}
		else if (argc >= 6 && (strcasecmp(argv[2], "hex_write")==0))
		{
			StorageHexWrite = TRUE;	
		}
		else if (argc == 6 && (strcasecmp(argv[2], "hex_read") == 0))
		{
			StorgeHexRead = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "lock") == 0))
		{
			StorageAreaLock = TRUE;
		}
		else if (argc == 6 && (strcasecmp(argv[2], "unlock") == 0))
		{
			StorageAreaUnLock = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 2;
		}
	}
	else if (strcasecmp(argv[1], "/f") == 0)
	{
		if (argc == 8 && (strcasecmp(argv[2], "set_temp_points") == 0))
		{
			SmartFanTempSet = TRUE;
		}
		else if (argc == 8 && (strcasecmp(argv[2], "set_pwm_points") == 0))
		{
			SmartFanPWMSet = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_temp_points") == 0))
		{
			SmartFanTempGet = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_pwm_points") == 0))
		{
			SmartFanPWMGet = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_temp_source") == 0))
		{
			SmartFanTempGetSrc = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_temp_source") == 0))
		{
			SmartFanTempSetSrc = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_mode") == 0))
		{
			SmartFanModeGet = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_mode") == 0))
		{
			SmartFanModeSet = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 3;
		}
	}
	else if (strcasecmp(argv[1], "/i") == 0)
	{
		if (argc == 4 && (strcasecmp(argv[2], "get_bd_info") == 0))
		{
			GetStringA = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 4;
		}
	}
	else if (strcasecmp(argv[1], "/v") == 0)
	{
		if (argc == 4 && (strcasecmp(argv[2], "get_voltage") == 0))
		{
			GetVoltageMonitor = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 5;
		}
	}
	else if (strcasecmp(argv[1], "/e") == 0)
	{
		if (argc == 4 && (strcasecmp(argv[2], "get_error_log") == 0))
		{
			GetErrorLog = TRUE;
		}
		else if (argc == 3 && (strcasecmp(argv[2], "get_cur_error_log") == 0))
		{
			GetCurrentPosErrorLog = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_bmc_error_code") == 0))
		{
			GetErrorNumberDescription = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 6;
		}
	}
	else if (strcasecmp(argv[1], "/x") == 0)
	{
		if (argc == 3 && (strcasecmp(argv[2], "get_excep_desc") == 0))
		{
			GetExceptionDescription = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 7;
		}
	}
	else if (strcasecmp(argv[1], "/g") == 0)
	{
		if (argc == 4 && (strcasecmp(argv[2], "get_direction_cap") == 0)) /***GPIO****/
		{
			GPIOGetDirectionCaps = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_direction") == 0))
		{
			GPIOGetDirection = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_direction") == 0))
		{
			GPIOSetDirection = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_level") == 0))
		{
			GPIOGetLevel = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_level") == 0))
		{
			GPIOSetLevel = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 8;
		}
	}
	else if (strcasecmp(argv[1], "/d") == 0)
	{
		if (argc == 4 && (strcasecmp(argv[2], "get_value") == 0))
		{
			GetValue = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 9;
		}
	}
	else if (strcasecmp(argv[1], "/b") == 0)
	{
		if (argc == 4 && (strcasecmp(argv[2], "get_bkl_enable") == 0))
		{
			VgaGetBacklightEnable = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_bkl_enable") == 0))
		{
			VgaSetBacklightEnable = TRUE;
		}
		else if (argc == 4 && (strcasecmp(argv[2], "get_bkl_value") == 0))
		{
			VgaGetBacklightBrightness = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "set_bkl_value") == 0))
		{
			VgaSetBacklightBrightness = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			help_condition = 10;
		}
	}
	else if (strcasecmp(argv[1], "/i2c") == 0)
	{
		if (argc == 3 && (strcasecmp(argv[2], "bus_cap") == 0))
		{
			IsI2CCap = TRUE;
		}
		else if (argc > 2 && (strcasecmp(argv[2], "probe_device") == 0))
		{
			if (argc == 4)
			{
				IsI2CProb = TRUE;
				I2CFuncArgs.BusID = atoi(argv[3]);

				if (I2CFuncArgs.BusID < 1 || I2CFuncArgs.BusID > 4)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
					eRet = -3;
				}

			}
			else
			{
				printf("Wrong arguments \n");
				printf("\nUsage :\n");
				printf("  semautil /i2c  probe_device [bus id]\n");

				printf("\n  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
				eRet = -3;
			}
		}
		else if ((argc > 2) && (strcasecmp(argv[2], "write_raw") == 0))
		{
			if (argc > 6)
			{
				int i;
				IsI2CWrRaw = TRUE;
				I2CFuncArgs.BusID = atoi(argv[3]);
				I2CFuncArgs.Address = string_to_hex(argv[4]);
				if( I2CFuncArgs.Address < 0 || I2CFuncArgs.Address > 127)
				{
					return -3;
				}
				I2CFuncArgs.Address = I2CFuncArgs.Address << 1;
				I2CFuncArgs.nByteCnt = atoi(argv[5]);
				I2CFuncArgs.pBuffer = calloc(I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (I2CFuncArgs.pBuffer == NULL)
				{
					return -3;
				}
				if (I2CFuncArgs.BusID < 1 || I2CFuncArgs.BusID > 5)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
					eRet = -3;
				}

				if (argc != I2CFuncArgs.nByteCnt + 6)
				{
					printf("Wrong arguments \n");
					printf("\nUsage :\n");
					printf("  semautil /i2c  write_raw	 [bus id] [address] [length] byte0 byte1 byte2 ...\n");

					printf("  [Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
					eRet = -3;
				}
				else
				{
					if (I2CFuncArgs.nByteCnt > 29 || I2CFuncArgs.nByteCnt <= 0)
					{
						printf("\nInvalid Size maximum write size is 29 bytes, min 1 byte.\n");
						eRet = -3;
						return eRet;
					}
					for (i = 0; i < I2CFuncArgs.nByteCnt; i++)
					{
						((unsigned char*)(I2CFuncArgs.pBuffer))[i] = string_to_hex(argv[6 + i]);
					}
				}
			}
			else
			{
				printf("Wrong arguments \n");
				printf("\nUsage :\n");
				printf("  semautil /i2c  write_raw	 [bus id] [address] [length] byte0 byte1 byte2 ...\n");

				printf("  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (strcasecmp(argv[2], "read_raw") == 0))
		{
			if (argc == 6)
			{
				IsI2CReRaw = TRUE;
				I2CFuncArgs.BusID = atoi(argv[3]);
				I2CFuncArgs.Address = string_to_hex(argv[4]);
				if( I2CFuncArgs.Address < 0 || I2CFuncArgs.Address > 127)
				{
					return -3;
				}
				I2CFuncArgs.Address = I2CFuncArgs.Address << 1;
				I2CFuncArgs.nByteCnt = atoi(argv[5]); 
				if (I2CFuncArgs.nByteCnt > 32 || I2CFuncArgs.nByteCnt <= 0)
				{
					printf("\nInvalid Size maximum read size is 32 bytes, min 1 byte.\n");
					eRet = -3;
					return eRet;
				}
				I2CFuncArgs.pBuffer = calloc(I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (I2CFuncArgs.pBuffer == NULL)
				{
					return -3;
				}
				if (I2CFuncArgs.BusID < 1 || I2CFuncArgs.BusID > 5)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
					eRet = -3;
				}
			}
			else
			{
				printf("Wrong arguments \n");
				printf("\nUsage :\n");
				printf("  semautil /i2c  read_raw	 [bus id] [address] [length]\n");

				printf("\n  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (strcasecmp(argv[2], "read_xfer") == 0))
		{
			if (argc == 8 || argc == 7)
			{
				IsI2CReXf = TRUE;
				I2CFuncArgs.BusID = atoi(argv[3]);
				I2CFuncArgs.Address = string_to_hex(argv[4]);
				if( I2CFuncArgs.Address < 0 || I2CFuncArgs.Address > 127)
				{
					return -3;
				}
				I2CFuncArgs.Address = I2CFuncArgs.Address << 1;
				I2CFuncArgs.CmdType = atoi(argv[5]);
				I2CFuncArgs.cmd = string_to_hex(argv[6]);
				I2CFuncArgs.nByteCnt = atoi(argv[7 + argc - 8]);

				if (I2CFuncArgs.BusID < 1 || I2CFuncArgs.BusID > 5)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
					eRet = -3;
				}

				switch(I2CFuncArgs.CmdType)
				{
					case 1:
						I2CFuncArgs.cmd = I2CFuncArgs.cmd | (1 << 30);
						break;
					case 2:
						I2CFuncArgs.cmd = I2CFuncArgs.cmd;
						break;
					case 3:
						I2CFuncArgs.cmd = I2CFuncArgs.cmd | (2 << 30);
						break;
					default:
						printf("Invalid Command type\n");

						printf("  [Command Type]:\n");
						printf("    ID\tENCODED CMD ID\t\tDescription\n");
						printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
						printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
						printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
						eRet = -3;
						return eRet;
				}
				if (argc == 7 && I2CFuncArgs.CmdType != 1)
				{
					printf("Wrong arguments \n");
					printf("\nUsage :\n");
					printf(" EAPI_I2C_NO_CMD:\n");
					printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [read length]\n");
					printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
					printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [read length]\n");

					printf("\n  [Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
					eRet = -3;
					return eRet;
				}
				if (argc == 8 && I2CFuncArgs.CmdType == 1)
				{
					printf("Wrong arguments \n");
					printf("\nUsage :\n");
					printf(" EAPI_I2C_NO_CMD:\n");
					printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [read length]\n");
					printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
					printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [read length]\n");

					printf("\n  [Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
					eRet = -3;
					return eRet;
				}
				if (I2CFuncArgs.nByteCnt > 32 || I2CFuncArgs.nByteCnt <= 0)
				{
					printf("\nInvalid Size maximum read size is 32 bytes, min 1 byte.\n");
					eRet = -3;
					return eRet;
				}
				I2CFuncArgs.pBuffer = calloc(I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (I2CFuncArgs.pBuffer == NULL)
				{
					return -3;
				}
			}
			else
			{
				printf("Wrong arguments \n");
				printf("\nUsage :\n");
				printf(" EAPI_I2C_NO_CMD:\n");
				printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [read length]\n");
				printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
				printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [read length]\n");

				printf("\n  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");

				printf("  [Command Type]:\n");
				printf("    ID\tENCODED CMD ID\t\tDescription\n");
				printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
				printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
				printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (strcasecmp(argv[2], "write_xfer") == 0))
		{
			int count = 8;
			if (argc > 5)
			{
				I2CFuncArgs.BusID = atoi(argv[3]);
				I2CFuncArgs.Address = string_to_hex(argv[4]);
				if( I2CFuncArgs.Address < 0 || I2CFuncArgs.Address > 127)
				{
					return -3;
				}
				I2CFuncArgs.Address = I2CFuncArgs.Address << 1;
				I2CFuncArgs.CmdType = atoi(argv[5]);

				if (I2CFuncArgs.BusID < 1 || I2CFuncArgs.BusID > 5)
				{
					printf("Wrong arguments \n");
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
					eRet = -3;
					return eRet;
				}

				if (I2CFuncArgs.CmdType == 1)
				{
					count = 7;
				}
				else if(I2CFuncArgs.CmdType != 3 && I2CFuncArgs.CmdType != 2)
				{
					printf("Invalid Command type\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
					eRet = -3;
					return eRet;
				}
			}

			if (argc > count)
			{
				int i;
				I2CFuncArgs.cmd = string_to_hex(argv[6]);
				switch (I2CFuncArgs.CmdType)
				{

					case 1:
						I2CFuncArgs.cmd = (I2CFuncArgs.cmd | (1 << 30));
						break;
					case 2:
						I2CFuncArgs.cmd = I2CFuncArgs.cmd;
						break;
					case 3:
						I2CFuncArgs.cmd = (I2CFuncArgs.cmd | (2 << 30));
						break;
				}
				I2CFuncArgs.nByteCnt = atoi(argv[7 + count - 8]);

				I2CFuncArgs.pBuffer = calloc(I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (I2CFuncArgs.pBuffer == NULL)
				{
					return -3;
				}

				if (argc != I2CFuncArgs.nByteCnt + count)
				{
					printf("Wrong arguments \n");
					printf("\nUsage :\n");
					printf(" EAPI_I2C_NO_CMD:\n");
					printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [length] byte0 byte1 byte2 ...\n");
					printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
					printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [cmd] [length] byte0 byte1 byte2 ...\n");

					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
					eRet = -3;
				}
				else
				{
					if (I2CFuncArgs.nByteCnt > 29 || I2CFuncArgs.nByteCnt <= 0)
					{
						printf("\nInvalid Size maximum write size is 29 bytes, min 1 byte.\n");
						eRet = -3;
						return eRet;
					}
					IsI2CWrXf = TRUE;
					for (i = 0; i < I2CFuncArgs.nByteCnt; i++)
					{
						((unsigned char*)(I2CFuncArgs.pBuffer))[i] = string_to_hex(argv[count + i]);
					}
				}
			}
			else
			{
				printf("Wrong arguments \n");
				printf("\nUsage :\n");

				printf(" EAPI_I2C_NO_CMD:\n");
				printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [length] byte0 byte1 byte2 ...\n");
				printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
				printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [cmd] [length] byte0 byte1 byte2 ...\n");

				printf("  \n[Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");

				printf("  [Command Type]:\n");
				printf("    ID\tENCODED CMD ID\t\tDescription\n");
				printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
				printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
				printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
				eRet = -3;
			}
		}
		else if (argc == 3 && (strcasecmp(argv[2], "get_status") == 0))
		{
			IsI2CSts = TRUE;
		}
		else
		{
			printf("Wrong arguments \n");
			printf("\n- Generic I2C Read/Write:\n");
			printf("  1. semautil /i2c  bus_cap\n");
			printf("  2. semautil /i2c  probe_device [bus id]\n");
			printf("  3. semautil /i2c  write_raw	 [bus id] [address] [length] byte0 byte1 byte2 ...\n");
			printf("  3. semautil /i2c  read_raw	 [bus id] [address] [length]\n");
			printf("  4. semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [read lentgh]\n");
			printf("  5. semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [cmd] [length] byte0 byte1 byte2 ...\n");
			printf("  6. semautil /i2c  get_status\n");

			printf("  \n[Bus Id]:\n");
			printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
			printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
			printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
			printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");

			printf("  [Command type]:\n");
			printf("    ID\tENCODED CMD ID\t\tDescription\n");
			printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
			printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
			printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 16 bits CMD\n");
			eRet = -3;
		}
	}

	else if (strcasecmp(argv[1], "/src") == 0)
        {
                if (argc == 3 && (strcasecmp(argv[2], "get_src") == 0))
                {
                        GetBiosSource = TRUE;
                }
                else if (argc == 4 && (strcasecmp(argv[2], "set_src") == 0))
                {
                        SetBiosSource = TRUE;
                        srcdata = atoi(argv[3]);
                        if (srcdata > 3 || srcdata < 0)
                        {
                                SetBiosSource = FALSE;
                                help_condition = 12;
                        }
                 }
		else if(argc ==3 && (strcasecmp(argv[2], "get_bios_status")== 0))
                {
                        GetBiosStatus = TRUE;
                }

                else
                {
                        help_condition = 12;
                }
        }


	else
	{
		ShowHelp(help_condition);
		eRet = -3;
	}
	if (help_condition != 0)
	{
		ShowHelp(help_condition);
		eRet = -3;
	}
	return eRet;
}

int main(int argc , char* argv[])
{

	if (ParseArgs(argc, argv) < 0)
	{
		//printf("Argument Failed \n");
		return -1;
	}

	if (DispatchCMDToSEMA(argc,argv) < 0)
	{
		printf("Failed\n");
		return -2;
	}

	EApiLibUnInitialize();
	return 0;
}


