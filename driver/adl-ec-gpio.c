#include <linux/err.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/gpio/driver.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/version.h>
#include "adl-ec.h"

#define ADL_BMC_OFS_GPIO_IN_PORT                 0x88
#define ADL_BMC_OFS_GPIO_OUT_PORT                0x86
#define ADL_BMC_OFS_GPIO_DIR			 0x84
#define ADL_BMC_OFS_GPIO_IN_PORT_EXT             0x89
#define ADL_BMC_OFS_GPIO_OUT_PORT_EXT            0x87
#define ADL_BMC_OFS_GPIO_DIR_EXT		 0x85
#define ADL_BMC_OFS_GPIO_CAP			 0x15

#define GET_GPIO_DIR    _IOR('a','1',uint32_t *)

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
	u8 gpio_in, gpio_out, dir,cap=0;

	if(offset >= 0x08)
	{
		ret= adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1,EC_REGION_1);
		cap=cap & (1<<4);
	}

	if(cap!=0)
       	{
		u8 offset_ext;
       	    	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_IN_PORT_EXT, &gpio_in, 1, EC_REGION_1); 
           	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT_EXT, &gpio_out, 1, EC_REGION_1); 
            	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);

		 gpio_in = (dir & gpio_in) | (~dir & gpio_out);
		 offset_ext=offset-8;
		 return !!(gpio_in & (1 << offset_ext));


        }
	else
	{
        	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_IN_PORT, &gpio_in, 1, EC_REGION_1); 
        	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1); 
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		
		gpio_in = (dir & gpio_in) | (~dir & gpio_out);
		return !!(gpio_in & (1 << offset));
	}

}

static void adl_gpio_set(struct gpio_chip *chip, unsigned int offset,
				int value)
{
	u8 gpio_out,cap=0;
	int ret;

	if(offset >= 0x08)
        {
                ret= adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1,EC_REGION_1);
                cap=cap & (1<<4);
        }

	if(cap!=0)
	{
		u8 offset_ext;
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT_EXT, &gpio_out, 1, EC_REGION_1);
		
		offset_ext=offset-8;
		
		if (value == 1)
                        gpio_out = gpio_out | (1 << offset_ext);
                else
                        gpio_out = gpio_out & ~(1 << offset_ext);
		
		adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_OUT_PORT_EXT, &gpio_out, 1, EC_REGION_1);
	}
        else
	{
      		
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1);
		
	       	if (value == 1)
                        gpio_out = gpio_out | (1 << offset);
                else
                        gpio_out = gpio_out & ~(1 << offset);
	
       		adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_OUT_PORT, &gpio_out, 1, EC_REGION_1);
	}

}

static int adl_gpio_direction_input(struct gpio_chip *gc,
					   unsigned int nr)
{
	u8 dir,cap=0;
	int ret;

	if(nr >= 0x08)
        {
                ret= adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1,EC_REGION_1);
                cap=cap & (1<<4);
        }

	if(cap!=0)
	{
		u8 nr_ext;
        	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
		
		nr_ext=nr-8;
		
		dir=dir|(1 << nr_ext);
		
		adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);

	}
	else
	{
 		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		
	       	dir = dir | (1 << nr);
	
	       	adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
	}
	return 0;

}

static int adl_gpio_direction_output(struct gpio_chip *gc,
					    unsigned int nr, int value)
{
	u8 dir,cap=0;
	int ret;
	
	if(nr >= 0x08)
        {
                ret= adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1,EC_REGION_1);
                cap=cap & (1<<4);
        }

	if(cap!=0){
		u8 nr_ext;
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
		
		nr_ext=nr-8;
		dir=dir & ~(1 << nr_ext);
	
		adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
	
		adl_gpio_set(gc,  nr_ext, 0);

	}
	else
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		
		dir = dir & ~(1 << nr);
		
		adl_bmc_ec_write_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		
		adl_gpio_set(gc,  nr, 0);
	}

	return 0;
}
static int adl_gpio_request(struct gpio_chip *chip, unsigned nr)
{
        return 0;
}

static int adl_gpio_get_direction(uint32_t* value)
{
	uint8_t dir,cap = 0;
	int ret;
	ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1,EC_REGION_1);
        cap = cap & (1<<4);
	
	if(cap!=0)
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR_EXT, &dir, 1, EC_REGION_1);
		*value |= (dir & 0x0F);
		*value <<= 8;
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		*value |= dir;
	}
	else
	{
		ret = adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_DIR, &dir, 1, EC_REGION_1);
		*value |= dir;
	}
	return 0;
}

static const struct gpio_chip adl_gpio_gc = {
	.label = "adl-ec-gpio",
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
	.label = "adl-ec-gpio",
	.owner = THIS_MODULE,
	.get = adl_gpio_get,
	.set = adl_gpio_set,
	.direction_input = adl_gpio_direction_input,
	.direction_output = adl_gpio_direction_output,
	.request = adl_gpio_request,
	.ngpio = 12,
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

static long int ioctl (struct file *file, unsigned cmd, unsigned long arg)
{
	int RetVal;
	uint32_t gpio_dir;
	switch(cmd)
        {
                case GET_GPIO_DIR:
                {

                        if((RetVal = copy_from_user(&gpio_dir,(uint32_t *)arg,sizeof(gpio_dir)))!=0)
                        {
                                return -EFAULT;
                        }

                        RetVal=adl_gpio_get_direction(&gpio_dir);

                        if((RetVal = copy_to_user((uint32_t *) arg,&gpio_dir,sizeof(gpio_dir)))!=0)
                        {
                                return -EFAULT;
                        }
                }
                break;

                default:
                        return -1;
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
        printk("%s\n", __func__);

	first_dev = alloc_chrdev_region(&devdrv, 0, 1, "gpio_adl");
	if(first_dev < 0)
	{
		return -1;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
	class_adl_gpio = class_create("gpio_adl");
#else
	class_adl_gpio = class_create(THIS_MODULE, "gpio_adl");
#endif

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
	
	ret= adl_bmc_ec_read_device(ADL_BMC_OFS_GPIO_CAP, &cap, 1,EC_REGION_1);
        cap=cap & (1<<4);
	
	if(cap==0)
	gpio->gp = adl_gpio_gc;
	else 
	gpio->gp = adl_gpio_gc_ext;

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
		.name	= "adl-ec-gpio",
	},
};
module_platform_driver(adl_ec_gpio_driver);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("ADLINK");
MODULE_DESCRIPTION("ADLINK EC GPIO Driver");
