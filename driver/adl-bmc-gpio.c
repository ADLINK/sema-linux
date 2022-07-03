#include <linux/err.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/gpio/driver.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include "adl-bmc.h"

#define ADL_BMC_OFS_GPIO_IN_PORT                 0x88
#define ADL_BMC_OFS_GPIO_OUT_PORT                0x86
#define ADL_BMC_OFS_GPIO_DIR			 0x84

dev_t devdrv;
struct class *class_adl_gpio;
int first_dev;
struct cdev cdev; 

struct adl_bmc_gpio {
	struct gpio_chip gp;
};

int flag;

static int adl_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	int ret;
	u8 gpio_in, gpio_out, dir;
        ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_IN_PORT, &gpio_in, 1, EC_REGION_1); 
        ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1); 

        ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);

	gpio_in = (dir & gpio_in) | (~dir & gpio_out);

	return !!(gpio_in & (1 << offset));
}

static void adl_gpio_set(struct gpio_chip *chip, unsigned int offset,
				int value)
{
	u8 gpio_out;
	int ret;
	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1);
	if (value == 1)
	     gpio_out = gpio_out | (1 << offset);
	else
	     gpio_out = gpio_out & ~(1 << offset);
	adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1);
}

static int adl_gpio_direction_input(struct gpio_chip *gc,
					   unsigned int nr)
{
	u8 dir;
	int ret;
        ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
        dir = dir | (1 << nr);
        adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);

	return 0;
}

static int adl_gpio_direction_output(struct gpio_chip *gc,
					    unsigned int nr, int value)
{
	u8 dir;
	int ret;
        ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
        dir = dir & ~(1 << nr);
        adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
	adl_gpio_set(gc,  nr, 0);
	return 0;
}
static int adl_gpio_request(struct gpio_chip *chip, unsigned nr)
{
        return 0;
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

int open(struct inode *inode, struct file *file)
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

int release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open,
	.release = release,
};

static int adl_ec_gpio_probe(struct platform_device *pdev)
{
	struct adl_bmc_gpio *gpio;
	int ret;
        printk("%s\n", __func__);

	first_dev = alloc_chrdev_region(&devdrv, 0, 1, "gpio_adl");
	if(first_dev < 0)
	{
		return -1;
	}

	class_adl_gpio = class_create(THIS_MODULE, "gpio_adl");
	if (IS_ERR(class_adl_gpio)) {
		printk("Error in Class_create\n");
	}

	cdev_init(&cdev, &fops);

	if(cdev_add(&cdev, devdrv, 1))
	{
		printk("Error in Cdev_add\n");
	}

	if(IS_ERR(device_create(class_adl_gpio, NULL, devdrv, NULL, "gpio_adl")))
	{
		printk("Error in device create\n");
	}

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->gp = adl_gpio_gc;
	gpio->gp.parent = pdev->dev.parent;

	ret = devm_gpiochip_add_data(&pdev->dev, &gpio->gp, gpio);
	if (ret < 0) {
		dev_err(&pdev->dev, "Could not register gpiochip, %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, gpio);
        printk("%s\n", __func__);
	return 0;
}

static int adl_ec_gpio_remove(struct platform_device *pdev)
{
	device_destroy(class_adl_gpio, devdrv);
	class_destroy(class_adl_gpio);
	cdev_del(&cdev);
	unregister_chrdev(devdrv, "gpio_adl");
	return 0;
}

static struct platform_driver adl_ec_gpio_driver = {
	.probe = adl_ec_gpio_probe,
	.remove = adl_ec_gpio_remove,
	.driver = {
		.name	= "adl-bmc-gpio",
	},
};
module_platform_driver(adl_ec_gpio_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ADLINK");
MODULE_DESCRIPTION("ADLINK EC GPIO Driver");
