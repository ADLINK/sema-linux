/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */
/* Watchdog timer inside of BMC , part of a mfd device */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/watchdog.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#include "adl-ec.h"
#include "adl-bmc.h"

#if __has_include("/etc/redhat-release")
        #define CONFIG_REDHAT
#endif

#define WDT_TIMEOUT   		10
#define WDT_MIN_TIMEOUT   	1
#define WDT_MAX_TIMEOUT   	65535

#define SET_WDT_TIMEOUT    	_IOWR('a', '1',uint16_t *)
#define GET_WDT_TIMEOUT       	_IOWR('a', '2',uint16_t *)
#define TRIGGER_WDT       	_IOWR('a', '3',uint16_t *)
#define STOP_WDT_TIMEOUT       	_IOWR('a', '4',uint16_t *)

struct adl_bmc_wdt {
	struct adl_bmc_dev *adl_dev;
	struct watchdog_device wdt;
	struct kobject *kobj_ref;
};

struct mutex wdt_lock;
dev_t devdrv;
struct class *class_adl_wdt;
int first_dev;
struct cdev cdev;
struct adl_bmc_wdt *awdt;
int flag;
unsigned short time_count;

static int adl_bmc_wdt_write(unsigned short val)
{
	int ret = 0;
	unsigned char buff[2];

	if(awdt->adl_dev->con_type == EC)
	{
		buff[0] = val & 0xFF;
		buff[1] = val >> 8;

		if((ret = adl_bmc_ec_write_device(ADL_BMC_OFS_SET_WD_CURR, ((u8*)(&buff[0])), 2, EC_REGION_1)) == 0)
		{
			ret = adl_bmc_ec_write_device(ADL_BMC_OFS_SET_WD, ((u8*)(&buff[0])), 2, EC_REGION_1);
		}
	}
	else
	{
		buff[0] = val >> 8;
        	buff[1] = val & 0xff;
		time_count = val;
        	ret = adl_bmc_i2c_write_device(awdt->adl_dev, ADL_BMC_CMD_SET_WD, 2,  &buff[0]);
	}

	if (ret < 0) {
		debug_printk("Write error: %d\n", ret);
		return ret;
	}

	return ret;

}

static int adl_bmc_wdt_set_timeout(unsigned short timeout)
{
        debug_printk("set timeout.................\n");
       	
        return adl_bmc_wdt_write(timeout);
}

static unsigned int adl_bmc_wdt_get_timeleft(unsigned short* val)
{
	int ret; 
	unsigned char buff[32];

	debug_printk("get timeleft.................\n");
 
 	if(awdt->adl_dev->con_type ==EC){
 		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_SET_WD_CURR, (u8*)buff, 2, EC_REGION_1);
 		*val = (unsigned short)buff[1] << 8 | buff[0];
 	}
 	else{
		ret = adl_bmc_i2c_read_device(awdt->adl_dev, ADL_BMC_CMD_SET_WD, 0,  (void *)buff);
		*val = (unsigned short)buff[0] << 8 | buff[1];
		if(*val != 0x00)
		{
			*val = time_count;
		}
	}
	if (ret < 0) {
		debug_printk("Write error: %d\n", ret);
		return ret;
	}
	return 0;
}

static int adl_bmc_wdt_stop(unsigned short value)
{
	debug_printk("stop........................\n");
	
	time_count = 0;
	return adl_bmc_wdt_write(value);
}

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
	unsigned char buff[32];
	memset(buff, 0, 32);

	if(awdt->adl_dev->con_type == EC)
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_SET_PWD, (u8*)buff, 2, EC_REGION_1);	
    		timeout = buff[0] | buff[1] << 8;
	}
	else
	{
		ret = adl_bmc_i2c_read_device(NULL, ADL_BMC_CMD_SET_PWD, 2, (void *)buff);
    		timeout = (buff[0] << 8) | buff[1];
	}

	if (ret < 0)
		return ret;

	return sprintf(buf, "%hu\n", timeout);
}

static ssize_t sysfs_store_PwrUpWDog(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned char buff[32];
	int ret;
	int timeout;

	sscanf(buf, "%d", &timeout);
	if(timeout > 65535) {
                return -EINVAL;
        }
	debug_printk("%s value: %u\n", __func__,timeout);

	if(awdt->adl_dev->con_type == EC)
	{
		unsigned char conf_data[4] = {0};
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
	}
	else
	{
		buff[0] = timeout >> 8;
        	buff[1] = timeout & 0xFF;

        	ret = adl_bmc_i2c_write_device(NULL, ADL_BMC_CMD_SET_PWD, 2, (void *)buff);
	}
	if (ret < 0)
		return ret;

	return count;
}

static int open(struct inode *inode, struct file *file)
{
        if(flag == 0)
        {
                flag = 1;
                return 0;
        }
        else
        {
                return -EBUSY;
        }
}

static int release(struct inode *inode, struct file *file)
{
        flag = 0;
        return 0;
}

static long int ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
	int ret;
	uint16_t timeout;

	// Only GET_WDT_TIMEOUT should avoid copy_from_user
	if (cmd != GET_WDT_TIMEOUT) {
		if ((ret = copy_from_user(&timeout, (uint16_t*)arg, sizeof(timeout))) != 0) {
			return -EFAULT;
		}
	}

	mutex_lock(&wdt_lock);
	switch (cmd)
	{
	case SET_WDT_TIMEOUT:
		ret = adl_bmc_wdt_set_timeout(timeout);
		if (ret < 0){
			mutex_unlock(&wdt_lock);
			return -EIO;  // or appropriate error
		}
		break;

	case GET_WDT_TIMEOUT:
		ret = adl_bmc_wdt_get_timeleft(&timeout);
		if (ret < 0){
			mutex_unlock(&wdt_lock);
			return -EIO;
		}

		if (copy_to_user((uint16_t*)arg, &timeout, sizeof(timeout)) != 0)
		{
			mutex_unlock(&wdt_lock);
			return -EFAULT;
		}
		break;

	case TRIGGER_WDT:
		ret = adl_bmc_wdt_set_timeout(timeout);
		if (ret < 0){
			mutex_unlock(&wdt_lock);
			return -EIO;
		}
		break;

	case STOP_WDT_TIMEOUT:
		ret = adl_bmc_wdt_stop(timeout);
		if (ret < 0){
			mutex_unlock(&wdt_lock);
			return -EIO;
		}
		break;

	default:
		mutex_unlock(&wdt_lock);
		return -ENOTTY;  // Invalid ioctl command
	}

	mutex_unlock(&wdt_lock);
	return 0;
}

struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = open,
        .unlocked_ioctl = ioctl,
        .release = release,
};

struct kobj_attribute attr0 = __ATTR_RO(wdt_min_timeout);
struct kobj_attribute attr1 = __ATTR_RO(wdt_max_timeout);
struct kobj_attribute attr2 = __ATTR(PwrUpWDog, 0660, sysfs_show_PwrUpWDog, sysfs_store_PwrUpWDog);

static int adl_bmc_wdt_probe(struct platform_device *pdev)
{
	int ret = 0;

	awdt = kzalloc(sizeof(*awdt), GFP_KERNEL);
	if (!awdt)
		return -ENOMEM;

	awdt->adl_dev = dev_get_drvdata(pdev->dev.parent);

        first_dev = alloc_chrdev_region(&devdrv, 0, 1, "watchdog_adl");
       	if(first_dev < 0)
        {
               	return -1;
        }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,14,0) && defined(CONFIG_REDHAT)
	 class_adl_wdt = class_create("watchdog_adl");
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
	 class_adl_wdt = class_create("watchdog_adl");
#else
	 class_adl_wdt = class_create(THIS_MODULE, "watchdog_adl");
#endif
       
        if (IS_ERR(class_adl_wdt)) {
               	printk("Error in Class_create\n");
        }

        cdev_init(&cdev, &fops);

        if(cdev_add(&cdev, devdrv, 1))
        {
                printk("Error in Cdev_add\n");
        }

        if(IS_ERR(device_create(class_adl_wdt, NULL, devdrv, NULL, "watchdog_adl")))
        {
               	printk("Error in device create\n");
        }

	/**Mutex Init**/
	mutex_init(&wdt_lock);

	platform_set_drvdata(pdev, awdt);

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
		debug_printk(KERN_ERR "Error sysfs attr2\n");
		return ret;
	}

	return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,11,0)
static void  adl_bmc_wdt_remove(struct platform_device *pdev)
#else
static int adl_bmc_wdt_remove(struct platform_device *pdev)
#endif
{
	sysfs_remove_file(kernel_kobj, &attr0.attr);
	sysfs_remove_file(kernel_kobj, &attr1.attr);
	sysfs_remove_file(kernel_kobj, &attr2.attr);
	kobject_put(awdt->kobj_ref);
	
	device_destroy(class_adl_wdt, devdrv);
        class_destroy(class_adl_wdt);
        cdev_del(&cdev);
        unregister_chrdev(devdrv, "watchdog_adl");
       
	debug_printk(" %s called.......\n", __func__);

	kfree(awdt);
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,11,0)
	return 0;
#endif
}


static struct platform_driver adl_bmc_wdt_driver = {
	.driver = {
		.name = "adl-bmc-wdt",
	},
	.probe = adl_bmc_wdt_probe,
	.remove = adl_bmc_wdt_remove,

};

module_platform_driver(adl_bmc_wdt_driver);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("ADLINK");
MODULE_DESCRIPTION("ADLINK BMC WDT Driver");
