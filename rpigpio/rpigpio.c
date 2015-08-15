/*
 * rpiogpio.c - PROC interface for reading BCM2835 GPIO register content
 *
 * Copyright (C) 2015 Tero Salminen

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <mach/platform.h>
#include <linux/io.h>
#include <linux/seq_file.h>

#define GPIOFSEL(x)  (0x00+(x)*4)
#define GPIOLEV(x)   (0x34+(x)*4)
#define GPIOEDS(x)   (0x40+(x)*4)

static const char * const gpio_alt_function[] = {
	"Input",	/* 0x0, 000 */
	"Output",	/* 0x1, 001 */
	"Alt 5",	/* 0x2: 010 */
	"Alt 4",	/* 0x3: 011 */
	"Alt 0",	/* 0x4: 100 */
	"Alt 1",	/* 0x5: 101 */
	"Alt 2",	/* 0x6: 110 */
	"Alt 3",	/* 0x7: 111 */
};

static const char * const gpio_level_str[] = {"Low", "High"};

static const char * const gpio_event_str[] = {"No event", "Event detected"};

static void print_gpio_from_reg(struct seq_file *buf,
				uint8_t *reg_base,
				int gpio,
				const char *prefix,
				const char * const *value_str)
{
	int bit;
	unsigned long regval;

	seq_printf(buf, "%s", prefix);

	/* 32 GPIO in single register */
	regval = ioread32(reg_base + ((gpio / 32) * 4));

	bit = (regval >> (gpio % 32)) & 0x1;

	seq_printf(buf, "%-8s  ", value_str[bit]);
}

static void print_reg_function_select(struct seq_file *buf,
				      uint8_t *base,
				      int gpio)
{
	int gpio_mode;
	unsigned long regval;

	/* 10 GPIOs in single register */
	regval = ioread32(base + GPIOFSEL(gpio / 10));

	seq_puts(buf, "Function: ");

	/* 3 bits for GPIO alternative function */
	gpio_mode = regval >> ((gpio % 10) * 3);
	gpio_mode &= 0x7;

	seq_printf(buf, "%-6s  ", gpio_alt_function[gpio_mode]);
}

static int rpigpio_proc_show(struct seq_file *buf, void *v)
{
	int retval = 0;
	int i;
	uint8_t *base = NULL;

	/* GPIO memory range 0x...0 -> 0x...B0 */
	base = ioremap(GPIO_BASE, 0xB0);

	/* 54 GPIOs */
	for (i = 0; i < 54; i++) {
		seq_printf(buf, "GPIO %2d: ", i);

		/* GPIO Function Select Registers (GPFSELn)  */
		print_reg_function_select(buf, base, i);

		/* GPIO Pin Level Registers (GPLEVn) */
		print_gpio_from_reg(
			buf,
			base + GPIOLEV(0),
			i,
			"Level: ",
			gpio_level_str);

		/* GPIO Event Detect Status Registers (GPEDSn) */
		print_gpio_from_reg(
			buf,
			base + GPIOEDS(0),
			i,
			"Event: ",
			gpio_event_str);
		seq_puts(buf, "\n");
	}

	iounmap(base);

	return retval;
}

static int rpigpio_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, rpigpio_proc_show, NULL);
}

static const struct file_operations rpigpio_proc_ops = {
	.owner      = THIS_MODULE,
	.open       = rpigpio_proc_open,
	.read       = seq_read,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int __init rpigpio_init(void)
{
	int retval = 0;
	struct proc_dir_entry *p_entry;

	p_entry = proc_create("rpigpio", 0, NULL, &rpigpio_proc_ops);
	if (IS_ERR_OR_NULL(p_entry))
		pr_err("rpigpio proc create failed\n");

	pr_info("rpigpio loaded\n");

	return retval;
}
module_init(rpigpio_init);

static void __exit rpigpio_exit(void)
{
	remove_proc_entry("rpigpio", NULL);
	pr_info("rpigpio unloaded\n");
}
module_exit(rpigpio_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("BCM2835 GPIO registry print");
MODULE_AUTHOR("Tero Salminen");
MODULE_VERSION("0.1");
