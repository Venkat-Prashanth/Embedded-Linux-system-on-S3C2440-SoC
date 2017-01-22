#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

struct virt_platform_data {
        char *name;
        int id;
        int bus_id;
        int virt_gpio_num;
};

static void virt_dev_release(struct device *dev)
{
	pr_info("virtual device released..\n");
}

/*
 * virtual device (256 bytes)
 */
static struct resource s3c_virt_resource[] = {
        [0] = {
                .start = 0x59100000,
                .end   = 0x59100000 + 0x00000100,
                .flags = IORESOURCE_MEM,
        }
};

static struct virt_platform_data my_virt_pdata = {
        .name           = "my virtual device node",
        .id             = 0,
        .bus_id         = -1,
        .virt_gpio_num  = 420
};

static struct platform_device s3c_device_virtual = {
        .name             = "s3c2440-virt",
        .id               = -1,
        .num_resources    = ARRAY_SIZE(s3c_virt_resource),
        .resource         = s3c_virt_resource,
        .dev            = {
		.release	= virt_dev_release,
                .platform_data  = &my_virt_pdata,
        },
};

static int mymod_init(void)
{
        printk(KERN_INFO "inserting my virtual device/node...\n");
	return platform_device_register(&s3c_device_virtual);
}

static void mymod_exit(void)
{
        printk(KERN_INFO "my virtual device/node corresponding driver ousted...\n");
        platform_device_unregister(&s3c_device_virtual);
}

module_init(mymod_init);
module_exit(mymod_exit);

MODULE_AUTHOR("techveda.org");
MODULE_DESCRIPTION("adding virtual device to a live kernel");
MODULE_LICENSE("GPL");
