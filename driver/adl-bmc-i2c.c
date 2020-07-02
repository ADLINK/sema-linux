// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for I2C bus inside of BMC , part of a mfd device
 *
 * Copyright (C) 2020 ADLINK Technology Inc.
 *
 */

#define pr_fmt(fmt) "adlink-i2c: " fmt

#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include "adl-bmc.h"

#define SLAVE_ADDR(x) ((x<<1) & 0xFE)

struct adlink_i2c_dev {
	struct device 		*dev;
	struct mutex 		i2c_lock;
	struct i2c_adapter 	adapter;
};

static int i2c_write (struct i2c_msg msg)
{
	int i, ret;
	unsigned char buf[32];

	buf[0] = SLAVE_ADDR(msg.addr);
	buf[1] = msg.len;
	buf[2] = 0x00;

	for(i=0; i<msg.len; i++)
		buf[i+3] = msg.buf[i];

	ret = adl_bmc_i2c_write_device(NULL, 0xC2, msg.len + 3, buf);
	if (ret < 0) {
		printk("i2c write error: %d\n", ret);
		return -ENODEV;
	}

	ret = adl_bmc_i2c_read_device(NULL, 0xC4, 0, buf);
	if (ret < 0) {
		printk("i2c read error: %d\n", ret);
		return -ENODEV;
	}

	debug_printk("write status = %x\n", buf[0]);
	ret = (buf[0] & 0x04) ? -ENODEV : 0;
	return ret;
}

static int i2c_read (struct i2c_msg msg)
{
	int ret;
	unsigned char buf[32];
	buf[0] = SLAVE_ADDR(msg.addr);

	if(msg.len > 0)
	{
		int i;

		buf[1] = 0x00;
		buf[2] = msg.len;

		ret = adl_bmc_i2c_write_device(NULL, 0xC2, msg.len + 3, buf);
		if (ret < 0) {
			printk("i2c write error: %d\n", ret);
			return -ENODEV;
		}

		ret = adl_bmc_i2c_read_device(NULL, 0xC4, 0, buf);
		if (ret < 0) {
			printk("i2c read error: %d\n", ret);
			return -ENODEV;
		}
		debug_printk("read status = %x\n", buf[0]);

		if(buf[0] & 0x04)
			return -ENODEV;

		ret = adl_bmc_i2c_read_device(NULL, 0xBF, 0, buf);
		if (ret < 0) {
			printk("i2c read error: %d\n", ret);
			return -ENODEV;
		}

		debug_printk("read request %x %d %x %x\n", msg.addr, msg.len, msg.flags, msg.buf[0]);
		for(i=0; i<ret; i++)
			msg.buf[i] = buf[i];

		return 0;
	}

	printk("Invalid read request\n");
	return -EINVAL;
}

static int adlink_i2c_xfer_msg (struct i2c_msg msg)
{
	int ret = -EOPNOTSUPP;

	if(msg.flags & I2C_M_RD)
		ret = i2c_read(msg);

	else if(msg.flags == 0 || msg.flags == 0x200)
		ret = i2c_write(msg);

	else
		printk("Unsupported xfer %x %x\n", msg.flags, msg.len);

	return ret;
}

static int adlink_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	int i, ret = -ENODEV;
	struct adlink_i2c_dev *adlink = i2c_get_adapdata(adap);

	mutex_lock(&adlink->i2c_lock);

	for(i=0; i<num; i++)
		ret = adlink_i2c_xfer_msg (msgs[i]);

	mutex_unlock(&adlink->i2c_lock);

	if(ret == 0)
		ret = num;

	return ret;
}

static u32 adlink_i2c_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm adlink_i2c_algo = {
	.functionality	= adlink_i2c_func,
	.master_xfer	= adlink_i2c_xfer,
};

static int adl_bmc_i2c_probe(struct platform_device *pdev)
{
	struct i2c_adapter *adap;
	struct adlink_i2c_dev *adlink;

	adlink = devm_kzalloc(&pdev->dev, sizeof(struct adlink_i2c_dev), GFP_KERNEL);
	if (!adlink)
		return -ENOMEM;

	adlink->dev = &pdev->dev;
	mutex_init(&adlink->i2c_lock);
	platform_set_drvdata(pdev, adlink);

	adap = &adlink->adapter;
	i2c_set_adapdata(adap, adlink);
	adap->owner = THIS_MODULE;
	adap->class = I2C_CLASS_DEPRECATED;
	strlcpy(adap->name, "ADLINK BMC I2C adapter", sizeof(adap->name));
	adap->algo = &adlink_i2c_algo;

	return i2c_add_adapter(adap);
}

static int adl_bmc_i2c_remove(struct platform_device *pdev)
{
	struct adlink_i2c_dev *adlink = platform_get_drvdata(pdev);

	i2c_del_adapter(&adlink->adapter);
	return 0;
}

static struct platform_driver adl_bmc_i2c_driver = {
	.driver = {
		.name	= "adl-bmc-i2c",
	},

	.probe		= adl_bmc_i2c_probe,
	.remove		= adl_bmc_i2c_remove,
};

module_platform_driver(adl_bmc_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adlink ");
MODULE_DESCRIPTION("ADLINK BMC I2C driver");
