/* Watchdog timer inside of BMC , part of a mfd device */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/watchdog.h>
#include <linux/platform_device.h>

#include "adl-bmc.h"


#define WDT_TIMEOUT   		10
#define WDT_MIN_TIMEOUT   	1
#define WDT_MAX_TIMEOUT   	65535

static struct adl_bmc_dev *adl_dev;

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started "
		"(default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");

struct adl_bmc_wdt {
	struct watchdog_device wdt;
	struct kobject *kobj_ref;
};

#define WDT_TO_ADL_BMC_WDT(_wdt) (container_of(_wdt, struct adl_bmc_wdt, wdt))

static int adl_bmc_wdt_write(struct watchdog_device *wdt, unsigned short val)
{
	int ret = 0;
	unsigned char buff[2];

	buff[0] = val & 0xFF;
	buff[1] = val >> 8;

	ret = adl_bmc_ec_write_device(ADL_BMC_OFS_SET_WD_CURR, ((u8*)(&buff[0])), 2, EC_REGION_1);
	ret = adl_bmc_ec_write_device(ADL_BMC_OFS_SET_WD, ((u8*)(&buff[0])), 2, EC_REGION_1);
	printk("came here\n");

	if (ret < 0) {
		debug_printk("EC write error: %d\n", ret);
		return ret;
	}
	

	return ret;

}

static int adl_bmc_wdt_set_timeout(struct watchdog_device *wdt, unsigned int timeout)
{
	debug_printk("set timeout.................\n");
	wdt->timeout = timeout;

	return adl_bmc_wdt_write(wdt, timeout);
}

static unsigned int adl_bmc_wdt_get_timeleft(struct watchdog_device *wdt)
{
	int ret; 
	unsigned int count;
	unsigned char buff[32];

	debug_printk("get timeleft.................\n");

	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_SET_WD, (u8*)buff, 2, EC_REGION_1);

	if (ret < 0) {
		debug_printk("i2c write error: %d\n", ret);
		return ret;
	}

	count = (unsigned int)buff[1] << 8 | buff[0];

	return count;
}

static int adl_bmc_wdt_start(struct watchdog_device *wdt)
{
	debug_printk("start.................\n");

	return adl_bmc_wdt_write(wdt, wdt->timeout);	
}

static int adl_bmc_wdt_ping(struct watchdog_device *wdt)
{
	return adl_bmc_wdt_write(wdt, wdt->timeout);
}

static int adl_bmc_wdt_stop(struct watchdog_device *wdt)
{
	wdt->timeout = 0;
	return adl_bmc_wdt_write(wdt, wdt->timeout);
}

static const struct watchdog_info adl_bmc_wdt_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_MAGICCLOSE | WDIOF_KEEPALIVEPING, 
	.identity = "ADL BMC Watchdog",
};

static const struct watchdog_ops adl_bmc_wdt_ops = {
	.owner           = THIS_MODULE,
	.start           = adl_bmc_wdt_start,
	.stop            = adl_bmc_wdt_stop,
	.ping            = adl_bmc_wdt_ping,
	.set_timeout     = adl_bmc_wdt_set_timeout,
	.get_timeleft    = adl_bmc_wdt_get_timeleft,
};


static ssize_t wdt_min_timeout_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	debug_printk("%s:%d called\n", __func__, __LINE__);
	return sprintf(buf, "%d\n", WDT_MIN_TIMEOUT);
}

static ssize_t wdt_max_timeout_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	debug_printk("%s:%d called\n", __func__, __LINE__);
	return sprintf(buf, "%d\n", WDT_MAX_TIMEOUT);
}

static ssize_t sysfs_show_PwrUpWDog(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int ret;
	unsigned short timeout;
	unsigned char buff[32] = {0};
	memset(buff, 0, 32);

	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_SET_PWD, (u8*)buff, 2, EC_REGION_1);	

	if (ret < 0)
		return ret;

    	timeout = buff[0] | buff[1] << 8;


	return sprintf(buf, "%hu\n", timeout);
}

static ssize_t sysfs_store_PwrUpWDog(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned char buff[32];
	int ret;
	int timeout;
	unsigned char conf_data[4] = {0};

	sscanf(buf, "%d", &timeout);

	debug_printk("%s value: %u\n", __func__,timeout);

	buff[1] = (timeout & 0xFF00) >> 8;
	buff[0] = timeout & 0xFF;

	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_SYSCFG, (u8*)conf_data,4, EC_REGION_1);
	if(ret < 0)
	{
		return ret;
	}

	if(timeout == 0)
	{
		conf_data[3] = conf_data[3] & ~(1 << 5);
		ret = adl_bmc_ec_write_device(ADL_BMC_OFS_SYSCFG, (u8*)conf_data,4, EC_REGION_1);
		if(ret < 0)
		{
			return ret;
		}
	}

	ret = adl_bmc_ec_write_device(ADL_BMC_OFS_SET_PWD, ((u8*)(&buff[0])), 2, EC_REGION_1);

	if(timeout != 0)
	{
		conf_data[3] = conf_data[3] | (1 << 5);
		if((ret = adl_bmc_ec_write_device(ADL_BMC_OFS_SYSCFG, (u8*)conf_data,4, EC_REGION_1)) < 0)
		{
			return ret;
		}
	}

	if (ret < 0)
		return ret;

	return count;
}

struct kobj_attribute attr0 = __ATTR_RO(wdt_min_timeout);
struct kobj_attribute attr1 = __ATTR_RO(wdt_max_timeout);
struct kobj_attribute attr2 = __ATTR(PwrUpWDog, 0660, sysfs_show_PwrUpWDog, sysfs_store_PwrUpWDog);

static int adl_bmc_wdt_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct adl_bmc_wdt *awdt;

	awdt = kzalloc(sizeof(*awdt), GFP_KERNEL);
	if (!awdt)
		return -ENOMEM;

	adl_dev = dev_get_drvdata(pdev->dev.parent);

	awdt->wdt.info               = &adl_bmc_wdt_info;
	awdt->wdt.ops                = &adl_bmc_wdt_ops;
	awdt->wdt.status             = 0;
	awdt->wdt.timeout            = 0;
	awdt->wdt.min_timeout        = WDT_MIN_TIMEOUT;
	awdt->wdt.max_timeout        = WDT_MAX_TIMEOUT;
	awdt->wdt.parent = &pdev->dev;

	watchdog_set_nowayout(&awdt->wdt, false); //nowayout);
	platform_set_drvdata(pdev, awdt);

	/*Adding Watchdog Stop on Reboot*/

	watchdog_stop_on_reboot(&awdt->wdt);

	ret = watchdog_register_device(&awdt->wdt);
	if (ret)
		return ret;


	awdt->kobj_ref = kobject_create_and_add("Capabilities", &pdev->dev.kobj);

	if((ret = sysfs_create_file(awdt->kobj_ref, &attr0.attr))) {
		debug_printk(KERN_ERR "Error sysfs attr0\n");
		return ret;
	}

	if((ret = sysfs_create_file(awdt->kobj_ref, &attr1.attr))) {
		debug_printk(KERN_ERR "Error sysfs attr1\n");
		return ret;
	}

	if((ret = sysfs_create_file(awdt->kobj_ref, &attr2.attr))) {
		debug_printk(KERN_ERR "Error sysfs attr1\n");
		return ret;
	}

	return 0;
}

static int adl_bmc_wdt_remove(struct platform_device *pdev)
{
	struct adl_bmc_wdt *awdt = platform_get_drvdata(pdev);

	sysfs_remove_file(kernel_kobj, &attr0.attr);
	sysfs_remove_file(kernel_kobj, &attr1.attr);
	sysfs_remove_file(kernel_kobj, &attr2.attr);
	kobject_put(awdt->kobj_ref);

	debug_printk(" %s called.......\n", __func__);
	watchdog_unregister_device(&awdt->wdt);

	kfree(awdt);
	return 0;
}


static struct platform_driver adl_bmc_wdt_driver = {
	.driver = {
		.name = "adl-bmc-wdt",
	},
	.probe = adl_bmc_wdt_probe,
	.remove = adl_bmc_wdt_remove,

};

module_platform_driver(adl_bmc_wdt_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adlink ");
MODULE_DESCRIPTION("Watchdog Timer driver");
