#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <errorcodes.h>
#include <eapi.h>
#include <conv.h>
#include <eapi.h>


char*			ExeName;
uint8_t	SetWatchdog, TriggerWatchdog, StopWatchdog, WatchDogCap,IsPwrUpWDogStart, IsPwrUpWDogStop;
uint8_t	StorageCap, StorageAreaRead, StorageAreaWrite;
uint8_t	SmartFanTempSet, SmartFanTempGet, SmartFanTempSetSrc, SmartFanTempGetSrc, SmartFanPWMSet;
uint8_t	SmartFanModeGet, SmartFanModeSet, SmartFanPWMGet;
uint8_t	GetStringA, GetValue, GetVoltageMonitor;
uint8_t	VgaGetBacklightEnable, VgaSetBacklightEnable, VgaGetBacklightBrightness, VgaSetBacklightBrightness;
uint8_t	GPIOGetDirectionCaps, GPIOGetDirection, GPIOSetDirection, GPIOGetLevel, GPIOSetLevel;
uint8_t GetErrorLog, GetErrorNumberDescription, GetCurrentPosErrorLog, GetExceptionDescription;

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
		printf("	semautil /h\n");
	}
	if (condition == 1 || condition == 0)
	{
		printf("- Watch Dog:\n");
		printf("  1. semautil /w get_cap\n");
		printf("  2. semautil /w start <sec> \n");
		printf("  3. semautil /w trigger\n");
		printf("  4. semautil /w stop\n");
		printf("  5. semautil /w pwrup_enable\n");
		printf("  6. semautil /w pwrup_disable\n\n");
	}
	if (condition == 2 || condition == 0)
	{
		printf("Storage:\n");
		printf("  1. semautil /s get_cap\n");
		printf("  2. semautil /s read  <Address> <Length> \n");
		printf("       Note: Address and Length should be  4Bytes aligned \n");
		printf("  3. semautil /s write <Address> <string/value> <Length> \n");
		printf("       Note: Address and Length should be  4Bytes aligned \n\n");
	}
	if (condition == 3 || condition == 0)
	{
		printf("- Smart FAN control:\n");
		printf("  1. semautil /f set_temp_points <FanID> <Level1> <Level2> <Level3> <level4> \n");
		printf("       FanID    index (0:CPU fan, #1-3: system fan #1-3)\n");
		printf("  2. semautil /f set_pwm_points  <FanID> <PWMLevel1> <PWMLevel2> <PWMLevel3> <PWMlevel4> \n");
		printf("       FanID    index (0:CPU fan, #1-3: system fan #1-3)\n");
		printf("  3. semautil /f get_temp_points <FanID> \n");
		printf("       FanID    index (0:CPU fan, #1-3: system fan #1-3)\n");
		printf("  4. semautil /f get_pwm_points  <FanID> \n");
		printf("       FanID    index (0:CPU fan, #1-3: system fan #1-3)\n");
		printf("  5. semautil /f set_temp_source <FanID> <TempSrc>\n");
		printf("       FanID    index (0:CPU fan, #1-3: system fan #1-3)\n");
		printf("       TempSrc   0 - CPU and 1 - Board\n");
		printf("  6. semautil /f get_temp_source <FanID> \n");
		printf("       FanID     index (0:CPU fan, #1-3: system fan #1-3)\n");
		printf("  7. semautil /f get_mode        <FanID> \n");
		printf("       FanID     index (0:CPU fan, 1-3: system fan #1-3)\n");
		printf("  8. semautil /f set_mode        <FanID> <Mode>\n");
		printf("       FanID     index (0:CPU fan, #1-3: system fan #1-3)\n");
		printf("       Mode	  mode  (0:Auto, 1: Off, 2: On, 3: Soft)\n\n");
	}
	if (condition == 4 || condition == 0)
	{
		printf("- System monitor-Board Info:\n");
		printf("  1. semautil /i get_bd_info <eapi ID> \n");
		printf("       1  : Board manufacturer name \n");
		printf("       2  : Board name \n");
		printf("       3  : Board serial number\n");
		printf("       4  : Board BIOS revision\n");
		printf("       5  : Board bootloader revision\n");
		printf("       6  : Board restart event\n");
		printf("       7  : HW revision \n");
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
		printf("  1. semautil /v get_voltage <Channel> \n");
		printf("       Channel 0-15\n");
	}
	if (condition == 6 || condition == 0)
	{
		printf("Error log:\n");
		printf("  1. semautil /e get_error_log       <Position>\n");
		printf("       Position 0-31\n");
		printf("  2. semautil /e get_cur_error_log\n");
		printf("  3. semautil /e get_bmc_error_code\n");
	}
	if (condition == 7 || condition == 0)
	{
		printf("Exception Description :\n");
		printf("  1. semautil /x get_excep_desc\n");
	}
	if (condition == 8 || condition == 0)
	{
		printf("GPIO:\n");
		printf("  1. semautil /g get_direction_cap [id]\n");
		printf("  2. semautil /g get_direction     <GPIO Bit>\n");
		printf("  3. semautil /g set_direction     <GPIO Bit> <0 - Output or 1 - Input>\n");
		printf("  4. semautil /g get_level         <GPIO Bit>\n");
		printf("  5. semautil /g set_level         <GPIO Bit> <0 - Low or 1 - High>\n");
		printf("       GPIO set/write parameters:\n");
		printf("       GPIO Bit   0-15   \n");
		printf("        Note: GPIO access may not be available on all platforms\n");
	}
	if (condition == 9 || condition == 0)
	{
		printf("Board values:\n");
		printf("  1. semautil /d  get_value <EAPI ID> \n");
		printf("       1	:  Spec version\n");
		printf("       2	:  Boot counter value\n");
		printf("       3	:  Running time meter value\n");
		printf("       4	:  library version\n");
		printf("       5	:  hwmon CPU temp source\n");
		printf("       6	:  hwmon System temp source\n");
		printf("       7	:  hwmon voltage vcore\n");
		printf("       8	:  hwmon voltage 2v5\n");
		printf("       9	:  hwmon voltage 3v3\n");
		printf("       10	:  hwmon voltage vbat\n");
		printf("       11	:  hwmon voltage 5v\n");
		printf("       12	:  hwmon voltage 5vSB\n");
		printf("       13	:  hwmon voltage 12v\n");
		printf("       14	:  hwmon CPU Fan\n");   /// not working
		printf("       15	:  hwmon system Fan 1\n");
		printf("       16	:  Board powerup time\n"); /// not working
		printf("       17	:  Board restart event\n");
		printf("       18	:  Board capabilities\n");
		printf("       19	:  Board capabilities Ex\n");
		printf("       20	:  Board system minimum temperature\n");
		printf("       21	:  Board system maximum temperature\n");
		printf("       22	:  Board system start up temperature\n");
		printf("       23	:  Board CPU minimum temperature\n");
		printf("       24	:  Board CPU maximum temperature\n");
		printf("       25	:  Board CPU start up temperature\n");
		printf("       26	:  Board main current\n");
		printf("       27	:  hwmon voltage GFX vcore\n");
		printf("       28	:  hwmon voltage 1v05\n");
		printf("       29	:  hwmon voltage 1v5\n");
		printf("       30	:  hwmon voltage vin\n");
		printf("       31	:  hwmon system Fan 2\n");
		printf("       32	:  hwmon system Fan 3\n");
		printf("       33	:  Board 2nd system temperature\n");
		printf("       34	:  Board 2nd system minimum temperature\n");
		printf("       35	:  Board 2nd system maximum temperature\n");
		printf("       36	:  Board 2nd system start up temperature\n");
		printf("       37	:  Board power cycle\n");
		printf("       38	:  Board BMC flag\n");
		printf("       39	:  Board BMC status\n");
		printf("       40	:  Board IO current\n");
	}
	if (condition == 10 || condition == 0)
	{
		printf("LVDS Backlight control:\n");
		printf("  1. semautil /b  set_bkl_value   [Id] [Level (0-255)]\n");
		printf("  2. semautil /b  set_bkl_enable  [Id] [0 or 1]\n");
		printf("  3. semautil /b  get_bkl_value   [Id]\n");
		printf("  4. semautil /b  get_bkl_enable  [Id]\n");
		printf("       [ID]       LCD\n");
		printf("        0    EAPI_ID_BACKLIGHT_1\n");
		printf("        1    EAPI_ID_BACKLIGHT_2\n");
		printf("        2    EAPI_ID_BACKLIGHT_3\n");
	}
}


int DispatchCMDToSEMA(int argc,char *argv[])
{
	int ret  = 0;
	/* Board information*/
	uint32_t Id, Size, Value = 0;
	char BoardInfo[64];
	char ExcepDesc[1024];
	memset(BoardInfo, 0, sizeof(BoardInfo));			
	memset(ExcepDesc, 0, sizeof(ExcepDesc));
	uint8_t ExceptionCode;
	uint32_t Pos, ErrorNumber = 0;
	uint8_t  Flags[20], RestartEvent[20];
	uint32_t PwrCycles, Bootcount, Time;
	uint8_t Status[20];
	signed char CPUtemp[20], Boardtemp[20];
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
	int fid = 0, Level1, Level2, Level3, Level4, Tempsrc, fan_mode;
	/* Storage */
	uint32_t Offset, BufLen, ByteCnt;
	char *Buffer, memcap[1024];
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
			printf("Wrong arguments VgaGetBacklightEnable\n");
			exit(-1);
		}					
		bid = atoi(argv[3]);
		ret = EApiVgaGetBacklightEnable(bid, &enable);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiVgaGetBackLightEnable");
		} 
		if(enable)
			printf("Backlight set OFF\n");
		else
			printf("Backlight set ON\n");
	}
	if (VgaSetBacklightEnable)
	{
		if (argc != 5) {
			printf("Wrong arguments VgaSetBacklightEnable\n");
			exit(-1);
		}
		bid = atoi(argv[3]);
		enable = atoi(argv[4]);
		ret = EApiVgaSetBacklightEnable(bid, enable);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiVgaSetBackLightEnable");
		}
		printf("Backlight Enable Set\n");
	}
	if (VgaGetBacklightBrightness)
	{
		if (argc != 4) {
			printf("Wrong arguments VgaGetBacklightBrightness\n");
			exit(-1);
		}
		bid = atoi(argv[3]);
		ret = EApiVgaGetBacklightBrightness(bid, &brightness);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiVgaGetBackLightBrightness");
		}
		printf("Backlight Brightness: %u\n", brightness);
	}
	if(VgaSetBacklightBrightness)
	{
		if (argc != 5) {
			printf("Wrong arguments VgaSetBacklightBrightness\n");
			exit(-1);
		}
		bid = atoi(argv[3]);
		brightness = atoi(argv[4]);
		ret = EApiVgaSetBacklightBrightness(bid, brightness);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiVgaSetBackLightBrightness");
		}
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
			printf("get eapi information failed\n");
			errno_exit("EApiBoardGetValue");
		}
		printf("Value: %u\n", Value);
	}
	if (GetStringA)
	{
		printf("test\n");
		if (argc != 4) {
			printf("Wrong arguments GetStringA\n");
			exit(-1);
		}
		Id = atoi(argv[3]);
		Size = sizeof(BoardInfo);
		ret = EApiBoardGetStringA(Id, BoardInfo, &Size);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiBoardGetStringA");
		}
		printf("%s\n", BoardInfo);
	}
	if (WatchDogCap)
	{
		ret = EApiWDogGetCap(&MaxDelay, &MaxEventTimeout, &MaxResetTimeout);
		if (ret)
			errno_exit("EApiWDogGetCap");
		printf("MaxDelay\t: %u\nMaxEventTimeout : %u\nMaxResetTimeout : %u\n", MaxDelay, MaxEventTimeout, MaxResetTimeout);
	}
	if (SetWatchdog)
	{
		if (argc != 6) {
			printf("Wrong arguments SetWatchdog\n");
			exit(-1);
		}
		delay = atoi(argv[3]);
		EventTimeout = atoi(argv[4]);
		ResetTimeout = atoi(argv[5]);
		ret = EApiWDogStart(delay, EventTimeout, ResetTimeout);
		if (ret == EAPI_STATUS_RUNNING)
		{
			errno = EALREADY;
			errno_exit("EApiWDogStart");
		}

		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiWDogStart");
		}
		printf("Watchdog timeout is set\n");
	}
	if (TriggerWatchdog)				
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		ret = EApiWDogTrigger();
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiWDogTrigger");
		}
		printf("Watchdog Tiggered\n");
	}
	if (StopWatchdog)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		ret = EApiWDogStop();
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiWDogStop");
		}
		printf("Watchdog Stop Success\n");
	}

	if (IsPwrUpWDogStop)				 
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		ret = EApiPwrUpWDogStop();
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiPwrUpWDogStop");
		}
		printf("PowerUp Watchdog Stop Success\n");
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
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanSetTempSetpoints");
		}
		printf("Fan ID: %d, Temperature set points done!\n", fid);
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
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanGetTempSetpoints");
		}
		printf("Fan ID: %d\nLevel1: %d\nLevel2: %d\nLevel3: %d\nLevel4: %d\n", fid, Level1, Level2, Level3, Level4);

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
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanSetPWMSetpoints");
		}
		printf("Fan ID: %d, PWM points are set!\n", fid);
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
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanGetPWMSetpoints");
		}
		printf("Fan ID: %d\nLevel1: %d\nLevel2: %d\nLevel3: %d\nLevel4: %d\n", fid, Level1, Level2, Level3, Level4);
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
		printf("Fan ID: %d, set to Mode: %d\n", fid, fan_mode);
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
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanGetMode");
		}
		printf("Fan id: %d\nFan Mode: %d\n", fid, fan_mode);

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
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanSetTempSrc");
		}
		printf("Fan ID: %d, set to Temperature source: %d\n", fid, Tempsrc);
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
			printf("get eapi information failed\n");
			errno_exit("EApiSmartFanGetTempSrc");
		}
		printf("Fan ID: %d\nTemperature source: %d\n", fid, Tempsrc);
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
			printf("get eapi information failed\n");
			errno_exit("EApiStorageCap");
		}
		printf("Sorage Capabilities:\nStorage Size: %u\nBlock Length: %u\n", Storagesize, BlockLength);
	}


	if (StorageAreaRead)
	{
		if (argc != 5) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Id = EAPI_ID_STORAGE_STD;
		memset(memcap, 0, sizeof(memcap));
		Offset = atoi(argv[3]);
		ByteCnt = atoi(argv[4]);
		BufLen = sizeof(memcap);
		ret = EApiStorageAreaRead(Id, Offset, memcap, BufLen, ByteCnt);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiStorageAreaRead");
		}
		printf("Read Buffer: %s\n", memcap);
	}


	if (StorageAreaWrite)
	{
		if (argc != 5) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Id = EAPI_ID_STORAGE_STD;
		Offset = atoi(argv[3]);
		Buffer = argv[4];
		ByteCnt = strlen(Buffer);
		ret = EApiStorageAreaWrite(Id, Offset, Buffer, ByteCnt);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiStorageAreaWrite");
		}
		printf("Write buffer: %s\n", Buffer);
	}



	if (GPIOGetDirectionCaps)
	{
		if (argc != 4){
			printf("Wrong arguments\n");
			exit(-1);
		}
		uint32_t input , output;
		Id = atoi(argv[3]);
		ret = EApiGPIOGetDirectionCaps(Id, &input, &output);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiGPIOGetDirectionCaps");
		}
		printf("GPIO Capabilities:\nInput : %u\nOutput: %u\n", input, output);
	}


	if (GPIOGetDirection)
	{
		uint32_t bitmask=0, dir;
		if((strncmp(argv[2], "0x", 2) != 0) || !Conv_HexString2DWord(argv[2], &bitmask)) {
			printf("Invalid GPIO Bitmask input\n");
			exit(1);
		}
		ret = EApiGPIOGetDirection(0, bitmask, &dir);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiGPIOGetDirection");
		}
		printf("GPIO Direction : %u\n", dir);
	}


	if (GPIOSetDirection)
	{
		uint32_t bitmask=0, dir=0;
		if((strncmp(argv[2], "0x", 2) != 0) || !Conv_HexString2DWord(argv[2], &bitmask)) {
			printf("Invalid GPIO Bitmask input\n");
			exit(1);
		}

		if((strncmp(argv[3], "0x", 2) != 0) || !Conv_HexString2DWord(argv[3], &dir)) {
			printf("Invalid GPIO Direction Input\n");
			exit(1);
		}


		ret = EApiGPIOSetDirection(0, bitmask, dir);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiGPIOSetDirection");
		}
	}

	if (GPIOGetLevel)
	{
		uint32_t bitmask=0, val=0;
		if((strncmp(argv[2], "0x", 2) != 0) || !Conv_HexString2DWord(argv[2], &bitmask)) {
			printf("Invalid GPIO Bitmask input\n");
			exit(1);
		}

		ret = EApiGPIOGetLevel(0, bitmask, &val);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiGPIOGetLevel");
		}
		printf("Value: %u\n", val);
	}


	if (GPIOSetLevel)
	{
		uint32_t bitmask=0, val=0;
		if((strncmp(argv[2], "0x", 2) != 0) || !Conv_HexString2DWord(argv[2], &bitmask)) {
			printf("Invalid GPIO Bitmask input\n");
			exit(1);
		}

		if((strncmp(argv[3], "0x", 2) != 0) || !Conv_HexString2DWord(argv[3], &val)) {
			printf("Invalid GPIO Direction Input\n");
			exit(1);
		}

		ret = EApiGPIOSetLevel(0, bitmask, val);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiGPIOSetLevel");
		}
	}


	if (GetErrorLog)
	{
		if (argc != 4) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		Pos = atoi(argv[3]);
		ret = EApiBoardGetErrorLog (Pos, &ErrorNumber, Flags, RestartEvent, &PwrCycles, &Bootcount, &Time, Status, CPUtemp, Boardtemp);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiBoardGetErrororLog");
		}
		printf("ErrorNumber: %u\nFlags: %s\nRestartEvent: %s\nPwrCycles: %u\nBootcount: %u\nTime: %u\n", ErrorNumber, Flags, RestartEvent, PwrCycles, Bootcount, Time);
		printf("Status: %s\nCPUtemp: %s\nBoardtemp: %s\n", Status, CPUtemp, Boardtemp);
	}


	if (GetErrorNumberDescription)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		Pos = atoi(argv[2]);
		ret = EApiBoardGetErrorNumDesc(Pos, ExcepDesc, Size);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiBoardGetErrorNumDescLog");
		}
		printf("%s\n", ExcepDesc);
	}

	if (GetCurrentPosErrorLog)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		ret = EApiBoardGetCurPosErrorLog (&ErrorNumber, Flags, RestartEvent, &PwrCycles, &Bootcount, &Time, Status, CPUtemp, Boardtemp);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiBoardGetCurPosErrorLog");
		}
		printf("ErrorNumber: %u\nFlags: %s\nRestartEvent: %s\nPwrCycles: %u\nBootcount: %u\nTime: %u\n", ErrorNumber, Flags, RestartEvent, PwrCycles, Bootcount, Time);
		printf("Status: %s\nCPUtemp: %s\nBoardtemp: %s\n", Status, CPUtemp, Boardtemp);
	}

	if (GetExceptionDescription)
	{
		if (argc != 3) {
			printf("Wrong arguments\n");
			exit(-1);
		}
		Size = sizeof(ExcepDesc);
		ExceptionCode = atoi(argv[2]);
		ret = EApiBoardGetExcepDesc(ExceptionCode, ExcepDesc, Size);
		if (ret) {
			printf("get eapi information failed\n");
			errno_exit("EApiBoardGetExcepDesc");
		}
		printf("Exception description: %s", ExcepDesc);
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
			printf("get eapi information failed\n");
			errno_exit("EApiBoardGetVoltageMonitor");
		}
		printf("Description: %sVoltage: %u\n", Vmbuf, Voltage);
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
		else if (argc == 6 && strcasecmp(argv[2], "start") == 0)
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
			help_condition = 1;
		}
	}
	else if (strcasecmp(argv[1], "/s") == 0)
	{
		if (argc == 3 && (strcasecmp(argv[2], "get_cap") == 0))
		{
			StorageCap = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "read") == 0))
		{
			StorageAreaRead = TRUE;
		}
		else if (argc == 5 && (strcasecmp(argv[2], "write") == 0))
		{
			StorageAreaWrite = TRUE;
		}
		else
		{
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
			help_condition = 3;
		}
	}
	else if (strcasecmp(argv[1], "/i") == 0)
	{
		if (argc == 4 && (strcasecmp(argv[2], "get_bd_info") == 0))
		{
			GetStringA = TRUE;
			printf("getStringA enabled\n");
		}
		else
		{
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
		else if (argc == 3 && (strcasecmp(argv[2], "get_bmc_error_code") == 0))
		{
			GetErrorNumberDescription = TRUE;
		}
		else
		{
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
			help_condition = 10;
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


