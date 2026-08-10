/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */
#include <linux/err.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/gpio/driver.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include "adl-ec.h"

#if __has_include("/etc/redhat-release")
        #define CONFIG_REDHAT
#endif

#define ADL_BMC_OFS_GPIO_IN_PORT                 0x88
#define ADL_BMC_OFS_GPIO_OUT_PORT                0x86
#define ADL_BMC_OFS_GPIO_DIR			 0x84
#define ADL_BMC_OFS_GPIO_IN_PORT_EXT             0x89
#define ADL_BMC_OFS_GPIO_OUT_PORT_EXT            0x87
#define ADL_BMC_OFS_GPIO_DIR_EXT		 0x85
#define ADL_BMC_OFS_GPIO_CAP			 0x15

#define GET_GPIO_DIR    _IOR('a','1',uint32_t *)
#define GET_LEVEL 	_IOR('a', '2', int32_t *)
#define SET_LEVEL  	_IOWR('a', '3', struct gpiostruct *)
#define OP_DIRECTION  	_IOWR('a', '4', struct gpiostruct *)
#define IN_DIRECTION  	_IOWR('a', '5', int32_t *)

dev_t devdrv;
struct class *class_adl_gpio;
int first_dev;
struct cdev cdev; 

struct adl_bmc_gpio {
	struct gpio_chip gp;
};

struct gpiostruct{
        unsigned int gpio;
        int val;
};

struct mutex gpio_lock;
int flag;

static struct adl_bmc_dev *adl_dev;

static int adl_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	int ret;
	u8 gpio_in, gpio_out, dir, cap = 0;

	mutex_lock(&gpio_lock);
	if (offset >= 0x08)
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1, EC_REGION_1);
		if (ret < 0)
			goto Exit;

		cap = cap & (1 << 4);
	}

	if (cap != 0)
	{
		u8 offset_ext;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_IN_PORT_EXT, &gpio_in, 1, EC_REGION_1); 
		if (ret < 0)
			goto Exit;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT_EXT, &gpio_out, 1, EC_REGION_1); 
		if (ret < 0)
			goto Exit;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
		if (ret < 0)
			goto Exit;

		gpio_in = (dir & gpio_in) | (~dir & gpio_out);
		offset_ext = offset - 8;
		mutex_unlock(&gpio_lock);
		return !!(gpio_in & (1 << offset_ext));
	}
	else
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_IN_PORT, &gpio_in, 1, EC_REGION_1); 
		if (ret < 0)
			goto Exit;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1); 
		if (ret < 0)
			goto Exit;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		if (ret < 0)
			goto Exit;

		gpio_in = (dir & gpio_in) | (~dir & gpio_out);
		mutex_unlock(&gpio_lock);
		return !!(gpio_in & (1 << offset));
Exit:
		mutex_unlock(&gpio_lock);
		return ret;
	}
}

static void __adl_gpio_set(struct gpio_chip *chip, unsigned int offset, int value, bool lock_needed)
{
	u8 gpio_out, cap = 0;
	int ret;
	
	if (lock_needed)
		mutex_lock(&gpio_lock);

	if (offset >= 0x08)
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1, EC_REGION_1);
		if (ret < 0)
		{
			pr_err("Failed to read GPIO CAP: %d\n", ret);
			 if (lock_needed)
				mutex_unlock(&gpio_lock);
			return;
		}
		cap = cap & (1 << 4);
	}

	if (cap != 0)
	{
		u8 offset_ext;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT_EXT, &gpio_out, 1, EC_REGION_1);
		if (ret < 0)
		{
			pr_err("Failed to read GPIO OUT EXT: %d\n", ret);
			if (lock_needed)
				mutex_unlock(&gpio_lock);
			return;
		}

		offset_ext = offset - 8;

		if (value == 1)
			gpio_out |= (1 << offset_ext);
		else
			gpio_out &= ~(1 << offset_ext);

		ret = adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_OUT_PORT_EXT, &gpio_out, 1, EC_REGION_1);
		if (ret < 0)
			pr_err("Failed to write GPIO OUT EXT: %d\n", ret);
	}
	else
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1);
		if (ret < 0)
		{
			pr_err("Failed to read GPIO OUT: %d\n", ret);
			if (lock_needed)
				mutex_unlock(&gpio_lock);
			return;
		}

		if (value == 1)
			gpio_out |= (1 << offset);
		else
			gpio_out &= ~(1 << offset);

		ret = adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1);
		if (ret < 0)
			pr_err("Failed to write GPIO OUT: %d\n", ret);	
	}
	if (lock_needed)
		mutex_unlock(&gpio_lock);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,17,0)
static int adl_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
#else
static void adl_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
#endif
{
	__adl_gpio_set(chip, offset, value, true);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,17,0)
	return 0;
#endif
}

static int adl_gpio_direction_input(struct gpio_chip *gc, unsigned int nr)
{
	u8 dir, cap = 0;
	int ret;

	mutex_lock(&gpio_lock);
	if (nr >= 0x08) {
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to read GPIO CAP: %d\n", ret);
			goto Exit;
		}
		cap = cap & (1 << 4);
	}

	if (cap != 0) {
		u8 nr_ext;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to read GPIO DIR EXT: %d\n", ret);
			goto Exit;
		}

		nr_ext = nr - 8;
		dir |= (1 << nr_ext);

		ret = adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to write GPIO DIR EXT: %d\n", ret);
			goto Exit;
		}
	} else {
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to read GPIO DIR: %d\n", ret);
			goto Exit;
		}

		dir |= (1 << nr);

		ret = adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to write GPIO DIR: %d\n", ret);
			goto Exit;
		}
	}

Exit:
	mutex_unlock(&gpio_lock);
	return ret;
}


static int adl_gpio_direction_output(struct gpio_chip *gc,
                                     unsigned int nr, int value)
{
	u8 dir, cap = 0;
	int ret;

	mutex_lock(&gpio_lock);

	if (nr >= 8) {
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1, EC_REGION_1);
		if (ret < 0)
			goto Exit;

		if (cap & (1 << 4)) {
			u8 nr_ext = nr - 8;

			ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
			if (ret < 0)
				goto Exit;

			dir &= ~(1 << nr_ext);

			ret = adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
			if (ret < 0)
				goto Exit;

			__adl_gpio_set(gc, nr_ext, value, false);

			mutex_unlock(&gpio_lock);
			return 0;
		}
	}

	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
	if (ret < 0)
		goto Exit;

	dir &= ~(1 << nr);

	ret = adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
	if (ret < 0)
		goto Exit;

	__adl_gpio_set(gc, nr, value, false);

Exit:
	mutex_unlock(&gpio_lock);
	return ret;
}

static int adl_gpio_request(struct gpio_chip *chip, unsigned nr)
{
        return 0;
}

static int adl_gpio_get_direction(uint32_t *value)
{
	uint8_t dir, cap = 0;
	int ret;

	mutex_lock(&gpio_lock);

	*value = 0;

	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1, EC_REGION_1);
	if (ret < 0) {
		pr_err("Failed to read GPIO CAP: %d\n", ret);
		goto Exit;
	}
	cap &= (1 << 4);

	if (cap != 0) {
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to read GPIO DIR EXT: %d\n", ret);
			goto Exit;
		}

		*value |= ((uint32_t)(dir & 0x0F)) << 8;

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to read GPIO DIR: %d\n", ret);
			goto Exit;
		}

		*value |= dir;
	} else {
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		if (ret < 0) {
			pr_err("Failed to read GPIO DIR: %d\n", ret);
			goto Exit;
		}

		*value |= dir;
	}

Exit:
	mutex_unlock(&gpio_lock);
	return ret;
}


static const struct gpio_chip adl_gpio_gc = {
	.label = "adl-bmc-gpio",
	.owner = THIS_MODULE,
	.get = adl_gpio_get,
	.set = adl_gpio_set,
	.direction_input = adl_gpio_direction_input,
	.direction_output = adl_gpio_direction_output,
	.request = adl_gpio_request,
	.ngpio = 8,
	.base = -1,
};

static const struct gpio_chip adl_gpio_gc_ext = {
	.label = "adl-bmc-gpio",
	.owner = THIS_MODULE,
	.get = adl_gpio_get,
	.set = adl_gpio_set,
	.direction_input = adl_gpio_direction_input,
	.direction_output = adl_gpio_direction_output,
	.request = adl_gpio_request,
	.ngpio = 12,
	.base = -1,
};

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

static long int ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	
	uint32_t gpio_dir;
	int32_t gpionum, gpio_ret;
	struct gpiostruct data;

	switch (cmd) {
	case GET_GPIO_DIR:
	{
		int RetVal=0;
		RetVal = adl_gpio_get_direction(&gpio_dir);
		if (RetVal != 0)
			return RetVal;

		if (copy_to_user((uint32_t __user *)arg, &gpio_dir, sizeof(gpio_dir)) != 0)
			return -EFAULT;
		break;
	}
	case GET_LEVEL:
	{
		if (copy_from_user(&gpionum, (int32_t __user *)arg, sizeof(gpionum)) != 0)
			return -EFAULT;

		gpio_ret = adl_gpio_get(NULL, gpionum);

		if (copy_to_user((int32_t __user *)arg, &gpio_ret, sizeof(gpio_ret)) != 0)
			return -EFAULT;

		break;
	}
	case SET_LEVEL:
	{
		if (copy_from_user(&data, (struct gpiostruct __user *)arg, sizeof(data)) != 0)
			return -EFAULT;

		adl_gpio_set(NULL, data.gpio, data.val);
		break;
	}
	case OP_DIRECTION:
	{
		if (copy_from_user(&data, (struct gpiostruct __user *)arg, sizeof(data)) != 0)
			return -EFAULT;

		adl_gpio_direction_output(NULL, data.gpio, data.val);
		break;
	}
	case IN_DIRECTION:
	{
		if (copy_from_user(&gpionum, (int32_t  __user*)arg, sizeof(gpionum)) != 0)
			return -EFAULT;

		adl_gpio_direction_input(NULL, gpionum);
		break;
	}
	default:
		return -EINVAL;
	}

	return 0;
}



struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open,
	.unlocked_ioctl = ioctl,
	.release = release,
};

static int adl_ec_gpio_probe(struct platform_device *pdev)
{
	struct adl_bmc_gpio *gpio;
	int ret;
	u8 cap;

	adl_dev = dev_get_drvdata(pdev->dev.parent);

	if(adl_dev->con_type == EC) {
		debug_printk(KERN_INFO "%s\n", __func__);

		ret = alloc_chrdev_region(&devdrv, 0, 1, "gpio_adl");
		if (ret < 0) {
			debug_printk(KERN_ERR "Failed to allocate chrdev region\n");
			return ret;
		}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,14,0) && defined(CONFIG_REDHAT)
		class_adl_gpio = class_create("gpio_adl");
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
		class_adl_gpio = class_create("gpio_adl");
#else
		class_adl_gpio = class_create(THIS_MODULE, "gpio_adl");
#endif
		if (IS_ERR(class_adl_gpio)) {
			debug_printk(KERN_ERR "Error creating class\n");
			ret = PTR_ERR(class_adl_gpio);
			unregister_chrdev_region(devdrv, 1);
			return ret;
		}

		cdev_init(&cdev, &fops);
		ret = cdev_add(&cdev, devdrv, 1);
		if (ret) {
			debug_printk(KERN_ERR "Error adding cdev\n");
			class_destroy(class_adl_gpio);
			unregister_chrdev_region(devdrv, 1);
			return ret;
		}

		if (IS_ERR(device_create(class_adl_gpio, NULL, devdrv, NULL, "gpio_adl"))) {
			debug_printk(KERN_ERR "Error creating device\n");
			cdev_del(&cdev);
			class_destroy(class_adl_gpio);
			unregister_chrdev_region(devdrv, 1);
			return -EINVAL;
		}

		gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
		if (!gpio) {
			device_destroy(class_adl_gpio, devdrv);
			cdev_del(&cdev);
			class_destroy(class_adl_gpio);
			unregister_chrdev_region(devdrv, 1);
			return -ENOMEM;
		}

		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1, EC_REGION_1);
		if (ret < 0) {
			dev_err(&pdev->dev, "Failed to read GPIO CAP register\n");
			device_destroy(class_adl_gpio, devdrv);
			cdev_del(&cdev);
			class_destroy(class_adl_gpio);
			unregister_chrdev_region(devdrv, 1);
			return ret;
		}

		cap = cap & (1 << 4);
		gpio->gp = (cap == 0) ? adl_gpio_gc : adl_gpio_gc_ext;
		gpio->gp.parent = pdev->dev.parent;

		ret = devm_gpiochip_add_data(&pdev->dev, &gpio->gp, gpio);
		if (ret < 0) {
			dev_err(&pdev->dev, "Could not register gpiochip, %d\n", ret);
			device_destroy(class_adl_gpio, devdrv);
			cdev_del(&cdev);
			class_destroy(class_adl_gpio);
			unregister_chrdev_region(devdrv, 1);
			return ret;
		}

		platform_set_drvdata(pdev, gpio);
		mutex_init(&gpio_lock);
		debug_printk(KERN_INFO "%s\n", __func__);
	}
	return 0;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,11,0)
static void adl_ec_gpio_remove(struct platform_device *pdev)
#else
static int adl_ec_gpio_remove(struct platform_device *pdev)
#endif
{
	if(adl_dev->con_type == EC) {
		device_destroy(class_adl_gpio, devdrv);
		class_destroy(class_adl_gpio);
		cdev_del(&cdev);
		unregister_chrdev(devdrv, "gpio_adl");
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(6,11,0)
	return 0;
#endif
}

static struct platform_driver adl_ec_gpio_driver = {
	.probe = adl_ec_gpio_probe,
	.remove = adl_ec_gpio_remove,
	.driver = {
		.name	= "adl-bmc-gpio",
	},
};
module_platform_driver(adl_ec_gpio_driver);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("ADLINK");
MODULE_DESCRIPTION("ADLINK BMC GPIO Driver");
