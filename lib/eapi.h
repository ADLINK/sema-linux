/**
 * @file eapi.h
 * @author
 * @brief File containing the SEMA Library Application Programming Interface Function Definitions
 *
 *
 */
#ifndef __EAPI_H__
#define __EAPI_H__

#include <stdint.h>

#define EAPI_UINT32_C(x) ((uint32_t) (x))

#define EAPI_VER_CREATE(Version, Revision, Build) (\
			EAPI_UINT32_C(\
				(((Version )&UINT8_MAX) << 24) | \
				(((Revision)&UINT8_MAX) << 16) | \
				(((Build    )&UINT16_MAX) << 0)  \
			)\
		)
#define EAPI_VER 1
#define EAPI_REVISION 0


#define EAPI_VERSION EAPI_VER_CREATE(EAPI_VER, EAPI_REVISION, 0)

#define EAPI_ID_STORAGE_STD              EAPI_UINT32_C(0)

#define TRUE 1
#define FALSE 0





#define EAPI_STATUS_NOT_INITIALIZED   EAPI_UINT32_C(0xFFFFFFFF)
#define EAPI_STATUS_INITIALIZED       EAPI_UINT32_C(0xFFFFFFFE)
#define EAPI_STATUS_ALLOC_ERROR       EAPI_UINT32_C(0xFFFFFFFD)
#define EAPI_STATUS_ERROR             EAPI_UINT32_C(0xFFFFF0FF)
#define EAPI_STATUS_UNSUPPORTED	      EAPI_UINT32_C(0xFFFFFCFF)
#define EAPI_STATUS_INVALID_PARAMETER EAPI_UINT32_C(0xFFFFFEFF)
#define EAPI_STATUS_READ_ERROR        EAPI_UINT32_C(0xFFFFFAFF)
#define EAPI_STATUS_WRITE_ERROR       EAPI_UINT32_C(0xFFFFFAFE)
#define EAPI_STATUS_SUCCESS           EAPI_UINT32_C(0)
#define EAPI_STATUS_RUNNING           EAPI_UINT32_C(0xFFFFFEFA)
#define EAPI_STATUS_MORE_DATA         EAPI_UINT32_C(0xFFFFF9FF)
#define EAPI_STATUS_INVALID_BLOCK_LENGTH    EAPI_UINT32_C(0xFFFFFEFD)
#define EAPI_STATUS_INVALID_BLOCK_ALIGNMENT EAPI_UINT32_C(0xFFFFFEFE)



/* IDS */
#define EAPI_ID_BACKLIGHT_1           EAPI_UINT32_C(0)
#define EAPI_ID_BACKLIGHT_2           EAPI_UINT32_C(1)
#define EAPI_ID_BACKLIGHT_3           EAPI_UINT32_C(2)

/* Backlight Values */
#define EAPI_BACKLIGHT_SET_ON         EAPI_UINT32_C(0)
#define EAPI_BACKLIGHT_SET_OFF        EAPI_UINT32_C(0xFFFFFFFF)
#define EAPI_BACKLIGHT_SET_DIMEST     EAPI_UINT32_C(0)
#define EAPI_BACKLIGHT_SET_BRIGHTEST  EAPI_UINT32_C(255)


/**
 * @brief Function initializes the SEMA Library.
 *
 * This function initializes the Smart Embedded Management Agent ( SEMA ) Library.
 * This function confirms that whether the board is supported by this library and initializes
 * if board is supported, else it will return -1 with error information on errno.
 * @return 0 on success -1 on failure
 */

uint32_t EApiLibInitialize(void);

/**
 * @brief Function Uninitializes the SEMA Library.
 *
 * This function Uninitializes the Smart Embedded Management Agent ( SEMA ) Library.
 * @return 0 on success -1 on failure
 */
uint32_t EApiLibUnInitialize(void);

/*
 *
 * B O A R D
 *
 */

/**
 * @brief Function To Get Text information about the hardware platform.
 * @param Id 		 Name Id.
 * @param pBuffer 	 Destination Buffer
 * @param pBufLen	 Buffer length
 * @return 0 on success -1 on failure
 */
uint32_t EApiBoardGetStringA(uint32_t Id, char *pBuffer, uint32_t *pBufLen);

/**
 * @brief Function To Get information about the hardware platform in values.
 * @param Id Value 	Id.
 * @param pValue	Return Value.
 * @return 0 on success -1 on failure
 */
uint32_t EApiBoardGetValue(uint32_t Id, uint32_t *pValue);


/**
 * @brief Function To Get text information and values of platform's voltage.
 * @param id Voltage subscription id.
 * @param pVoltage pointer to a buffer that receives the value's data.
 * @param pBuf pointer to a buffer that receives the description string.
 * @return 0 on success -1 on failure
 */
uint32_t EApiBoardGetVoltageMonitor(uint32_t id, uint32_t *pVoltage, char *pBuf, uint32_t size);


/**
 * @brief Function To Get Error number history in the BMC.
 * @param buf buffer pointer
 * @return 0 on success -1 on failure
 */
uint32_t EApiBoardGetErrorLog (uint32_t position, uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time, uint8_t *Status, signed char *CPUtemp, signed char *Boardtemp);


uint32_t EApiBoardGetCurPosErrorLog (uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time, uint8_t *Status, signed char *CPUtemp, signed char *Boardtemp);

/**
 * @brief Function To Get text information of the Error number in the error log.
 * @return 0 on success -1 on failure
 */
uint32_t EApiBoardGetErrorNumDesc(uint32_t Pos, char *pBuf, uint32_t size);

/**
 * @brief Function To Get text information of the Exception code.
 * @param Exceptioncode The Exception Code.
 * @param pBuf Pointer to buffer that receives the description string.
 * @return 0 on success -1 on failure
 */
uint32_t EApiBoardGetExcepDesc(uint32_t Exceptioncode, char *pBuf, uint32_t size);


/*
 *
 * B A C K L I G H T
 *
 */

/**
 * @brief Returns the current Backlight Enable State.
 * @param id Backlight Identifier
 * @param pEnable Buffer Pointer that receives the current backlight state
 * @return 0 On Success -1 On Failure
 */
uint32_t EApiVgaGetBacklightEnable(uint32_t Id, uint32_t *pEnable);

/**
 * @brief Set the current Backlight Enable State to either ON / OFF.
 * @param id Backlight Identifier
 * @param Enable Backlight State to be updated
 * @return 0 On Success -1 On Failure
 */
uint32_t EApiVgaSetBacklightEnable(uint32_t Id, uint32_t Enable);

/**
 * @brief Returns the current Backlight Brightness level.
 * @param id Backlight Identifier
 * @param pBrightness Buffer Pointer that receives the current backlight brightness level
 * @return 0 On Success -1 On Failure
 */
uint32_t EApiVgaGetBacklightBrightness(uint32_t Id, uint32_t *pBright);

/**
 * @brief Set the current Backlight brightness level.
 * @param id Backlight Identifier
 * @param Brightness Backlight Brightness Level to be updated
 * @return 0 On Success -1 On Failure
 */
uint32_t EApiVgaSetBacklightBrightness(uint32_t Id, uint32_t Bright);



/*
 *
 * S T O R A G E
 *
 */
/**
 * @brief Function To Get the capabilities of the selected storage area.
 * @param pBuf pointer to the buffer that receives the storage area capabilities.
 * @return 0 on success -1 on failure
 */
uint32_t EApiStorageCap(uint32_t Id, uint32_t *pStorageSize, uint32_t *pBlockLength);

/**
 * @brief Function To Read data to the selected user data area.
 * @param Offset storage area start address offset in bytes. This value must be a multiple of block length.
 * @param Size Number of bytes to read.
 * @param pBuf Pointer to buffer that receives the reda data.
 * @return 0 on success -1 on failure
 */
uint32_t EApiStorageAreaRead(uint32_t Id, uint32_t Offset, void *pBuffer, uint32_t BufLen, uint32_t  Bytecnt);

/**
 * @brief Function To Read data to the selected user data area.
 * @param Offset storage area start address offset in bytes. This value must be a multiple of block length.
 * @param pBuf Pointer to a buffer conaining the data to be stored.
 * @param Length Number of bytes to write.
 * @return 0 on success -1 on failure
 */
uint32_t EApiStorageAreaWrite(uint32_t Id, unsigned short Offset, char *pBuf, unsigned short Length);

/*
 *
 *  W A T C H D O G
 *
 */

/**
 * @brief Get the capabilities of Watch Dog timer.
 * This function retrieves the capabilities of Watch Dog Timer like Maximum Allowed Timeout parameter.
 * @param pMaxTimeout Pointer that receives the Watch Dog Maximum Timeout capability.
 * @return 0 On Success, -1 On Failure
 */
uint32_t EApiWDogGetCap(uint32_t *pMaxDelay, uint32_t *pMaxEventTimeout, uint32_t *pMaxResetTimeout);

/**
 * @brief Start the Watch Dog Timer and set the parameters
 * This function configures the watchdog timeout value and start the timer and this supports only run time watchdog.
 * To change the timeout value, the watchdog must be stopped and then WDogStart must be called with
 * new value again.
 * @param Timeout Watchdog timeout interval in milliseconds to trigger a reset.
 * @return 0 On Success, -1 On Failure
 */
uint32_t EApiWDogStart(uint32_t delay, uint32_t EventTimeout, uint32_t ResetTimeout);

/**
 * @brief Trigger the watchdog timer
 * This function trigger the watchdog timer to reloads the timer value and this supports only run time watchdog.
 * @return 0 On Success, -1 On Failure
 */
uint32_t EApiWDogTrigger(void);

/**
 * @brief Stops the operation of watchdog timer.
 * This function stop the operation of watchdog timer and this supports only run time watchdog.
 * @return 0 On Success, -1 On Failure
 */
uint32_t EApiWDogStop(void);

/**
 * @brief Start the Power Up Watch Dog Timer with timeout value.
 * This function configures the powerup watchdog timeout value and this supports only power-up watchdog.
 * To change the timeout value, the watchdog must be stopped and then WDogStart must be called with
 * new value again.
 * @param Timeout Power-UP Watchdog timeout interval in milliseconds to trigger a reset.
 * @return 0 On Success, -1 On Failure
 */
uint32_t EApiPwrUpWDogStart(uint32_t Timeout);

/**
 * @brief Stops the operation of power-up watchdog timer
 * This function stops the operation of power-up watchdog timer and this supports only power-up watchdog.
 * @return 0 On Success, -1 On Failure
 */
uint32_t EApiPwrUpWDogStop(void);




/*
 *
 *  G P I O
 *
 */

/**
 * @brief Gets the Direction capabilities of GPIO.
 * Reads the direction capabilities of the current GPIO implementation from the selected GPIO interface.
 * The ports where both input and output bit masks are 1 are GPIOs. The direction of this ports
 * can be configured by GPIOSetDir.
 * @param pinput Pointer to the buffer that receives the bit mask of supported inputs.
 * @param poutput Pointer to the buffer that receives the bit mask of supported outputs.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiGPIOGetDirectionCaps(uint32_t Id, uint32_t *pInputs, uint32_t *pOutputs);

/**
 * @brief Reads the current configuration of the selected GPIO ports.
 * @param bitmask bitmask value of GPIO Ports pin selection.
 * @param dir Pointer to buffer that receives the direction of selected GPIO Ports.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiGPIOGetDirection(uint32_t Id, uint32_t Bitmask, uint32_t *pDirection);

/**
 * @brief Sets the configuration for the selected GPIO ports.
 * @param bitmask bitmask value of GPIO Ports pin selection. Ports only with the level 1 are processed.
 * @param dir Sets the direction of the selected GPIO ports.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiGPIOSetDirection(uint32_t Id, uint32_t Bitmask, uint32_t Direction);

/**
 * @brief Gets the level of the selected GPIO ports.
 * @param bitmask bitmask value of GPIO Ports pin selection. Ports only with the level 1 are processed.
 * @param pLevel Pointer to buffer that receives the level of selected GPIO Ports..
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiGPIOGetLevel(uint32_t Id, uint32_t Bitmask, uint32_t *pLevel);

/**
 * @brief Sets the level of the selected GPIO ports.
 * @param bitmask value of GPIO Ports pin selection. Ports only with the level 1 are processed.
 * @param level Sets the level of the selected GPIO ports.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiGPIOSetLevel(uint32_t Id, uint32_t Bitmask, uint32_t Level);



/*
 *
 * F A N
 *
 */

/**
 * @brief Set temperature setpoints (signed, degree celsius).
 * @param id fan id.
 * @param Level1 Temperature Level1.
 * @param Level2 Temperature Level2.
 * @param Level3 Temperature Level3.
 * @param Level4 Temperature Level4.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanSetTempSetpoints(int id, int Level1, int Level2, int Level3, int Level4);

/**
 * @brief Get temperature setpoints (signed, degree celsius).
 * @param id fan id.
 * @param pLevel1 Pointer to buffer that receives the tempearture level1.
 * @param pLevel2 Pointer to buffer that receives the tempearture level2.
 * @param pLevel3 Pointer to buffer that receives the tempearture level3.
 * @param pLevel4 Pointer to buffer that receives the tempearture level4.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanGetTempSetpoints(int id, int *pLevel1, int *pLevel2, int *pLevel3, int *pLevel4);

/**
 * @brief Set PWM setpoints (valid range 0..100).
 * @param id fan id.
 * @param Level1 PWM Level1.
 * @param Level2 PWM Level2.
 * @param Level3 PWM Level3.
 * @param Level4 PWM Level4.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanSetPWMSetpoints(int id, int pwm_Level1, int pwm_Level2, int pwm_Level3, int pwm_Level4);

/**
 * @brief Get PWM setpoints (valid range 0..100).
 * @param id fan id.
 * @param pwm_Level1 Pointer to buffer that receives the pwm level1.
 * @param pwm_Level2 Pointer to buffer that receives the pwm level2.
 * @param pwm_Level3 Pointer to buffer that receives the pwm level3.
 * @param pwm_Level4 Pointer to buffer that receives the pwm level4.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanGetPWMSetpoints(int id, int *pwm_Level1, int *pwm_Level2, int *pwm_Level3, int *pwm_Level4);

/**
 * @brief Get Fan Mode.
 * @param id fan id.
 * @param fan_mode Pointer to buffer that receives the fan mode.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanGetMode(int id, int *fan_mode);

/**
 * @brief Set Fan Mode.
 * @param id fan id.
 * @param fan_mode fan mode.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanSetMode(int id, int fan_mode);

/**
 * @brief Get Temperature source of specific fan id.
 * @param id fan id.
 * @param pTempsrc Pointer to buffer that receives the temperature source.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanGetTempSrc(int id, int *pTempsrc);

/**
 * @brief Set Temperature source of specific fan id.
 * @param id fan id.
 * @param Tempsrc temperature source id.
 * @return 0 On Success, -1 On Failure.
 */
uint32_t EApiSmartFanSetTempSrc(int id, int Tempsrc);

/*
 *
 * I 2 C
 *
 */
uint32_t EApiI2CWriteReadRaw(uint32_t Id, uint8_t Addr, void *pWBuffer, uint32_t WriteBCnt, void *pRBuffer, uint32_t RBufLen, uint32_t  ReadBCnt);

#define EAPI_ID_I2C_EXTERNAL       EAPI_UINT32_C(0) /* Baseboard I2C Interface
                                                     * required
                                                     */
#define EAPI_ID_I2C_LVDS_1         EAPI_UINT32_C(1) /* LVDS/EDP 1 Interface
                                                     * (optional)
                                                     */
#define EAPI_ID_I2C_LVDS_2         EAPI_UINT32_C(2) /* LVDS/EDP 2 Interface
                                                     * (optional)
                                                     */
#define SEMA_EAPI_ID_I2C_EXTERNAL_2        EAPI_UINT32_C(3) /* 2nd External I2C Interface
                                                     * (optional)
                                                     */
#define SEMA_EAPI_ID_I2C_EXTERNAL_3        EAPI_UINT32_C(4) /* 3th External I2C Interface
                                                     * (optional)
                                                     */
#define SEMA_C_I2C3             0x20000000                      ///< Group 0 bit 29: Ext. I2C bus #3 available
#define SEMA_C_I2C4             0x40000000                      ///< Group 0 bit 30: Ext. I2C bus #4 available
#define SEMA_C_BMC_TIVA         0x80000000                      ///< Group 0 bit 31: BMC is from TIVA type

//Ext. I2C defines
#define SEMA_EXT_IIC_BUS_1              0
#define SEMA_EXT_IIC_BUS_2              1
#define SEMA_EXT_IIC_BUS_3              2
#define SEMA_EXT_IIC_BUS_4              3

#endif

