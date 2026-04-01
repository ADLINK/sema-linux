// SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause)
/* Backlight inside of BMC , part of a mfd device */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/backlight.h>
#include <linux/slab.h>
#include <linux/version.h>

#include "adl-ec.h"
#include "adl-bmc.h"

#define ADL_BMC_MIN_BRIGHT 0
#define ADL_BMC_MAX_BRIGHT 255

#define ADL_BMC_BKL_ON	0
#define ADL_BMC_BKL_OFF	4

static struct mutex bklight_lock;
int bkl_cap;
struct adl_bmc_bklight {
	int brightness;
	struct adl_bmc_dev *adl_dev;
};

static int adl_bmc_update_brightness(struct backlight_device *bl, int brightness)
{
	int ret;
	struct adl_bmc_bklight *bklite = bl_get_data(bl);
	unsigned char buff[2];
	debug_printk("func: %s line: %d\n", __func__, __LINE__);

	buff[0] = brightness;
	
	if(bklite->adl_dev->con_type == EC)
	{
		ret = adl_bmc_ec_write_device(ADL_BMC_OFS_BKLIGHT_PWM, ((u8*)&buff[0]), 1, EC_REGION_1);
	}
	else
	{
		ret = adl_bmc_i2c_write_device(NULL, ADL_BMC_CMD_SET_BKLITE, 1,  &buff[0]);
	}

	if (ret < 0) {
		debug_printk("Write error: %d\n", ret);
		return ret;
	}

	bklite->brightness = brightness;
	bl->props.brightness = brightness;
	
	return 0;
}

static int last = 0;

static int adl_bmc_bklight_update_status(struct backlight_device *bl)
{
	int brightness = 0;
	int ret;
	const struct adl_bmc_bklight *bklite __maybe_unused = bl_get_data(bl);

	mutex_lock(&bklight_lock);

	if (bl->props.brightness < ADL_BMC_MIN_BRIGHT ||
		bl->props.brightness > ADL_BMC_MAX_BRIGHT) {
		dev_err(&bl->dev, "lcd brightness should be %d to %d.\n",
			ADL_BMC_MIN_BRIGHT, ADL_BMC_MAX_BRIGHT);
		mutex_unlock(&bklight_lock);
		return -EINVAL;
	}

	if (bl->props.power == ADL_BMC_BKL_OFF) {
		brightness = 0;
	}
	else if (last == ADL_BMC_BKL_OFF && bl->props.power == ADL_BMC_BKL_ON) {
		brightness = 200;
	}
	else if (last == ADL_BMC_BKL_ON && bl->props.power == ADL_BMC_BKL_ON) {
		brightness = bl->props.brightness;
	}
	else {
		dev_err(&bl->dev, "Unsupported Backlight enable state\n");
		mutex_unlock(&bklight_lock);
		return -EINVAL;
	}

	ret = adl_bmc_update_brightness(bl, brightness);

	if (ret)
	{
		mutex_unlock(&bklight_lock);
		return ret;
	}

	if (brightness == 0)
	{
		bl->props.power = ADL_BMC_BKL_OFF; //Full OFF
		last = ADL_BMC_BKL_OFF;
	}
	else
	{
		bl->props.power = ADL_BMC_BKL_ON; //Full ON
		last = ADL_BMC_BKL_ON;
	}

	mutex_unlock(&bklight_lock);
	return 0;
}


static int adl_bmc_bklight_get_brightness(struct backlight_device *bl)
{
	int ret;
	unsigned char brightness;
	unsigned char buff[2];
	const struct adl_bmc_bklight *bklite = bl_get_data(bl);

	memset(buff, 0, sizeof(buff));

	mutex_lock(&bklight_lock);

	if(bklite->adl_dev->con_type == EC)
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_BKLIGHT_PWM, (u8*)buff, 1, EC_REGION_1);
	}
	else
	{
		ret = adl_bmc_i2c_read_device(NULL, ADL_BMC_CMD_GET_BKLITE, 0,  buff);
	}

	if (ret < 0) {
		debug_printk("Read error: %d\n", ret);
		mutex_unlock(&bklight_lock);
		return ret;
	}

	brightness = buff[0];
	
	mutex_unlock(&bklight_lock);
	return brightness;
}

static const struct backlight_ops adl_bmc_bklight_ops = {
	.get_brightness	= adl_bmc_bklight_get_brightness,
	.update_status	= adl_bmc_bklight_update_status,
};


static int adl_bmc_bklight_probe(struct platform_device *pdev)
{	
	struct backlight_device *bl = 0;
	struct backlight_properties props;
	struct adl_bmc_bklight *bklite;
	unsigned char brightness = 0;
	int ret;
	unsigned char buff[2];

	bklite = devm_kzalloc(&pdev->dev, sizeof(*bklite), GFP_KERNEL);
	if(!bklite)
		return -ENOMEM;

	bklite->adl_dev = dev_get_drvdata(pdev->dev.parent);

	memset(&props, 0, sizeof(struct backlight_properties));
	props.type = BACKLIGHT_RAW;
	props.max_brightness = ADL_BMC_MAX_BRIGHT;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
	bl = devm_backlight_device_register(&pdev->dev, pdev->name,
	pdev->dev.parent, bklite, &adl_bmc_bklight_ops,
	&props);
#else
	bl = backlight_device_register(pdev->name,
	pdev->dev.parent, bklite, &adl_bmc_bklight_ops,
	&props);
#endif

	if (IS_ERR(bl)) {
		dev_err(&pdev->dev, "failed to register backlight device\n");
		return PTR_ERR(bl);
	}
	
	/**Mutex Init**/
	mutex_init(&bklight_lock);
	/*Check Backlight capability*/
	if (bklite->adl_dev->Bmc_Capabilities[0] & ADL_BMC_CAP_BKLIGHT) 
	{
		bkl_cap = 1;
		debug_printk("Backlight functionality is compatible for this platform\n");
	}
	else {
		bkl_cap = 0;
		debug_printk("Backlight functionality is not compatible for this platform\n");
		return -EINVAL;
	}
	
	memset(buff, 0, sizeof(buff));

	if (bklite->adl_dev->con_type == BMC)
	{
		ret = adl_bmc_i2c_read_device(NULL, ADL_BMC_CMD_GET_BKLITE, 0, buff);
	}
	else
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_BKLIGHT_PWM, (u8*)buff, 1, EC_REGION_1);
	}

	if (ret < 0) {
		debug_printk("i2c read error: %d\n", ret);
		return ret;
	}

	brightness = buff[0];
	bl->props.brightness = brightness;

	if (bl->props.brightness == ADL_BMC_MIN_BRIGHT) {
		bl->props.power = ADL_BMC_BKL_OFF;//4;
	}
	else if (bl->props.brightness > ADL_BMC_MIN_BRIGHT && bl->props.brightness <= ADL_BMC_MAX_BRIGHT) {
		bl->props.power = ADL_BMC_BKL_ON;//0;
	}

	adl_bmc_bklight_update_status(bl);

	platform_set_drvdata(pdev, bl);
	
	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,11,0)
static void adl_bmc_bklight_remove(struct platform_device *pdev)
#else
static int adl_bmc_bklight_remove(struct platform_device *pdev)
#endif
{
	struct backlight_device *bl = platform_get_drvdata(pdev);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,11,0)
        devm_backlight_device_unregister(&pdev->dev, bl);
#else
        backlight_device_unregister(bl);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(6,11,0)
	return 0;
#endif
}

static struct platform_driver adl_bmc_bklight_driver = {
	.driver = {
		.name	= "adl-bmc-bklight",
	},

	.probe		= adl_bmc_bklight_probe,
	.remove		= adl_bmc_bklight_remove,
};

module_platform_driver(adl_bmc_bklight_driver);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Adlink ");
MODULE_DESCRIPTION("Backlight Driver");
