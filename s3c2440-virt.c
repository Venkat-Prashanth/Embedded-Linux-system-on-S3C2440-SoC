#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>



static int virt_drv_probe(struct platform_device *pdata)
{

	pr_info("virt_drv probe invoked....\n");
	pr_info("driver binded....\n");
	pr_info("pdata->name: %s\n",pdata->name);
//	virt_gpio_num = pdata->dev.platform_data
	pr_info("virt_gpio_num: %d\n",*(int *)((pdata->dev.platform_data) + 12));
	return 0;
}

static int virt_drv_remove(struct platform_device *pdata)
{
	pr_info("virt_drv remove invoked....\n");
	return 0;
}

/*
static const struct of_device_id tdispdrv_of_mtable[] = {
        { .compatible = "ti,tdispdrv", },
	{}
};

MODULE_DEVICE_TABLE(of, tdispdrv_of_mtable);
*/ 

static struct platform_driver virt_drv_driver = {
         .driver = {
                 .name    = "s3c2440-virt",
		 .owner   = THIS_MODULE,		
         },
         .probe = virt_drv_probe,
         .remove = virt_drv_remove,
};
 
static int mymod_init(void)
{
        printk(KERN_INFO "my virtual device/node corresponding driver inserted...\n");
        return platform_driver_register(&virt_drv_driver);
}

static void mymod_exit(void)
{
        printk(KERN_INFO "my virtual device/node corresponding driver ousted...\n");
        platform_driver_unregister(&virt_drv_driver);
}

module_init(mymod_init);
module_exit(mymod_exit);

MODULE_AUTHOR("techveda.org");
MODULE_DESCRIPTION("platfrom driver for the virtual device");
MODULE_LICENSE("GPL");
