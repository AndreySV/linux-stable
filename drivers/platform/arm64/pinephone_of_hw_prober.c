// SPDX-License-Identifier: GPL-2.0-only
/*
 * PinePhone Device Tree Hardware Prober
 *
 * Copyright (c) 2024 Andrey Skvortsov <andrej.skvortzov@gmail.com>
 */

#include <linux/errno.h>
#include <linux/i2c-of-prober.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#define DRV_NAME	"pinephone_of_hw_prober"

static const struct i2c_of_probe_cfg pinephone_i2c_probe_cfg = {
	.type = "magnetometer"
};

static int pinephone_of_hw_prober_probe(struct platform_device *pdev)
{
	int ret;
	struct i2c_of_probe_simple_ctx ctx = {
		.opts = NULL
	};

	ret = i2c_of_probe_component(&pdev->dev, &pinephone_i2c_probe_cfg, &ctx);
	/* Ignore unrecoverable errors */
	if (ret == -EPROBE_DEFER)
		return ret;

	return 0;
}

static struct platform_driver pinephone_of_hw_prober_driver = {
	.probe	= pinephone_of_hw_prober_probe,
	.driver	= {
		.name = DRV_NAME,
	},
};

static struct platform_device *pinephone_of_hw_prober_pdev;

static int pinephone_of_hw_prober_driver_init(void)
{
	int ret;

	if (!of_machine_is_compatible("pine64,pinephone-1.2"))
		return -ENODEV;

	ret = platform_driver_register(&pinephone_of_hw_prober_driver);
	if (ret)
		return ret;

	pinephone_of_hw_prober_pdev =
			platform_device_register_simple(DRV_NAME, PLATFORM_DEVID_NONE, NULL, 0);
	if (IS_ERR(pinephone_of_hw_prober_pdev))
		goto err;

	return 0;

err:
	platform_driver_unregister(&pinephone_of_hw_prober_driver);

	return PTR_ERR(pinephone_of_hw_prober_pdev);
}
module_init(pinephone_of_hw_prober_driver_init);

static void pinephone_of_hw_prober_driver_exit(void)
{
	platform_device_unregister(pinephone_of_hw_prober_pdev);
	platform_driver_unregister(&pinephone_of_hw_prober_driver);
}
module_exit(pinephone_of_hw_prober_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PinePhone device tree hardware prober");
MODULE_IMPORT_NS(I2C_OF_PROBER);
