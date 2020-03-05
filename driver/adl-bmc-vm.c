/*
 */



#include <linux/err.h>
#include <linux/of.h>
#include <linux/export.h>
#include <linux/platform_device.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>


#include "adl-bmc.h"




struct adl_bmc_vm_data {
	struct regulator_desc adl_bmc_vm_desc[16];
	struct regulator_dev *adl_bmc_vm_rdev[16];
	struct adl_bmc_dev *adl_dev;
	char name_arr[16][256];
	int cnt;

};


static int adl_bmc_vm_list_voltage(struct regulator_dev *rdev, unsigned selector)
{

	debug_printk("func: %s\n", __func__);
	return 0;
}

static int adl_bmc_vm_get_voltage(struct regulator_dev *rdev)
{
	int ret = 0;
	unsigned char buff_hm[32] = {0};
	unsigned char buff_gain[32] = {0};
	unsigned char cmd_hm;
	u64 vol_val, vol_val_fl;
	unsigned short gain;
	struct adl_bmc_vm_data *vm_data = rdev_get_drvdata(rdev);

	debug_printk("%s id: %d \n", __FUNCTION__, rdev->desc->id);
	
	if (rdev->desc->id < 8) {
		cmd_hm = ADL_BMC_CMD_RD_AIN0 + rdev->desc->id;
	}
	else if (rdev->desc->id >= 8 && rdev->desc->id < 16) {
		cmd_hm = ADL_BMC_CMD_RD_AIN8 + (rdev->desc->id - 8);
	}
	else
		return -EINVAL;

	debug_printk("I2C commmand: 0x%x\n", cmd_hm);

	
	/*read the hwmon register for voltage*/
        ret = adl_bmc_i2c_read_device(vm_data->adl_dev, cmd_hm, 2, buff_hm);
	if(ret != 2)
		return -EINVAL;

	/*read the gain value*/
        ret = adl_bmc_i2c_read_device(vm_data->adl_dev, ADL_BMC_CMD_GET_ADC_SCALE, 32, buff_gain);
	if (ret < 0)
		return ret;
	gain = (buff_gain[rdev->desc->id * 2] << 8) | buff_gain[(rdev->desc->id * 2) + 1];
	vol_val_fl = 3223 * gain;
	vol_val = vol_val_fl * (buff_hm[0] << 8 | buff_hm[1]);
	vol_val = vol_val / 1000000;

	return vol_val;
}

static struct regulator_init_data adl_bmc_initdata = {
	.constraints = {
		.always_on = 1,
	},
};

static struct regulator_ops adl_bmc_vm_ops = {
	.get_voltage = adl_bmc_vm_get_voltage,
	.list_voltage = adl_bmc_vm_list_voltage,

};


static int adl_bmc_vm_probe(struct platform_device *pdev)
{
	int ret, i, j, cnt = 0;
	unsigned char data[32];
	struct regulator_config config = { };

	struct device *dev = &pdev->dev;
	struct adl_bmc_vm_data *vm_data;


	vm_data = devm_kzalloc(dev, sizeof(struct adl_bmc_vm_data), GFP_KERNEL);
        if(!vm_data)
                return -ENOMEM;

	vm_data->adl_dev = dev_get_drvdata(pdev->dev.parent);

	debug_printk("adl dev1 is %p\n",  vm_data->adl_dev);
	/*check voltage monitor capability*/
	if (vm_data->adl_dev->Bmc_Capabilities[0] & ADL_BMC_CAP_VM)
		debug_printk("Voltage monitor is compatible for this platform\n");
	else { 
		debug_printk("Voltage monitor is compatible for this platform\n");
		return -EINVAL;
	}


	config.dev = &pdev->dev;
	config.driver_data = vm_data;
	config.init_data = &adl_bmc_initdata;


	debug_printk("probing....................\n");

	for (i = 0; i < 16; i++)
	{
		memset(vm_data->name_arr[i], 0, 256);

		data[0] = (unsigned char)i;
		ret = adl_bmc_i2c_write_device(vm_data->adl_dev, ADL_BMC_CMD_EXT_HW_DESC, 1, data);
		if (ret < 0)
			debug_printk("return failed  in write i=%d\n", i);
		ret = adl_bmc_i2c_read_device(vm_data->adl_dev, ADL_BMC_CMD_EXT_HW_DESC, 16, data);
		if (ret < 0)
			debug_printk("return failed in read i=%d\n", i);
		debug_printk("Buffer is %s\n", data);

		sprintf(vm_data->name_arr[i], "adl_bmc_reg%d_", i);
		data[15] = 0;
		strcat(vm_data->name_arr[i], data + 4);
		vm_data->adl_bmc_vm_desc[i].name = vm_data->name_arr[i];
		vm_data->adl_bmc_vm_desc[i].id = i;
		vm_data->adl_bmc_vm_desc[i].type = REGULATOR_VOLTAGE;
		vm_data->adl_bmc_vm_desc[i].ops = &adl_bmc_vm_ops;

		/*Check the number of voltage regulators supported*/
		if (strstr(data, "VCORE") != NULL)
			cnt++;
	}	

		vm_data->cnt = ADL_MAX_HW_MTR_INPUT - (cnt);
	debug_printk("Final Count is %d\n", vm_data->cnt);

	for (i = 0; i < vm_data->cnt; i++) 
        {
		vm_data->adl_bmc_vm_rdev[i] = devm_regulator_register(&pdev->dev, &vm_data->adl_bmc_vm_desc[i], &config);
		if (IS_ERR(vm_data->adl_bmc_vm_rdev[i])) {
			ret = PTR_ERR(vm_data->adl_bmc_vm_rdev[i]);
			pr_err("Failed to register regulator: %d\n", ret);
			goto ret_failed;
		}

	}

	platform_set_drvdata(pdev, vm_data);
	debug_printk("Returning probe\n");
	return 0;

ret_failed:
	for(j = 0; j < i; j++) {
		devm_regulator_unregister(&pdev->dev, vm_data->adl_bmc_vm_rdev[j]);
		debug_printk("In for loop\n");
	}

	return ret;
}

static int adl_bmc_vm_remove(struct platform_device *pdev)
{

	int i;
	struct adl_bmc_vm_data *vm_data = platform_get_drvdata(pdev);
	debug_printk("Remove...............\n");

	for (i = 0; i < vm_data->cnt; i++) 
	{
		devm_regulator_unregister(&pdev->dev, vm_data->adl_bmc_vm_rdev[i]);
	}

	devm_kfree(&pdev->dev, vm_data);

	return 0;
}

static struct platform_driver adl_bmc_vm_driver = {
	.driver		= {
		.name		= "adl-bmc-vm",
	},
	.probe		= adl_bmc_vm_probe,
	.remove 	= adl_bmc_vm_remove,

};

module_platform_driver(adl_bmc_vm_driver);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adlink ");
MODULE_DESCRIPTION("Voltage Monitor driver");
