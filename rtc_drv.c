#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/bcd.h>
#include <linux/cdev.h> 
#include <linux/slab.h> 

static void __iomem *rtc_base;
static void __iomem *bcd_base;

#define rRTCCON (* (volatile unsigned long *) (rtc_base + 0x00)) 
#define rTICNT (* (volatile unsigned long *) (rtc_base + 0x04)) 
#define rRTCALM (* (volatile unsigned long *) (rtc_base + 0x0a))

#define rBCDSEC (* (volatile unsigned long *) (bcd_base + 0x00))
#define rBCDMIN (* (volatile unsigned long *) (bcd_base + 0x04))
#define rBCDHOUR (* (volatile unsigned long *) (bcd_base + 0x08))
#define rBCDDATE (* (volatile unsigned long *) (bcd_base + 0xc))
#define rBCDDAY (* (volatile unsigned long *) (bcd_base + 0x10))
#define rBCDMON (* (volatile unsigned long *) (bcd_base + 0x14))
#define rBCDYEAR (* (volatile unsigned long *) (bcd_base + 0x18))

#define LEDOFF 0 
#define LEDON 1 
// # Define rtc_MAJOR 235 // static allocation 
#define rtc_MAJOR 0		// dynamic allocation 
#define rtc_MINOR 0
int devmajor = rtc_MAJOR;
int devminor = rtc_MINOR;
dev_t dev = 0;

// Device structure
// Device structure 
struct rtc_dev  {
	struct cdev cdev;
/*Char device structure */
};
struct TIME_RTC  {
	int year;
	int mon;
	int day;		// Week
	int date;
	int hour;
	int min;
	int sec;
};
struct rtc_dev *rtc_devices;
struct class *my_class;	// my_class
int rtc_open(struct inode *inode, struct file *filp) 
{
	rRTCCON = (0x0 << 3) | (0x0 << 2) | (0x0 << 1) | (0x1 << 0);
	
	return 0;
}

int rtc_release(struct inode *inode, struct file *filp)
{
	return 0;
}

int rtc_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
	       unsigned long arg) 
{
	return 0;
}

// You can also use rtc_write instead rtc_ioctl
ssize_t rtc_write(struct file * filp, const char __user * buf, size_t count,
		      loff_t * f_pos)
{
	struct TIME_RTC set_time;
	int len = sizeof(struct TIME_RTC);
	/*User space->kernel space */ 
	/*Failure to return the number of bytes not copied successfully 0 */
	if (copy_from_user(&set_time, buf, len))
	{
	printk("copy_to_user error \n");
	return -EFAULT;
	}
	rBCDHOUR = ((set_time.hour / 10) << 4) | (set_time.hour % 10);
	rBCDMIN = ((set_time.min / 10) << 4) | (set_time.min % 10);
	rBCDSEC = (set_time.sec / 10) << 4 | (set_time.sec % 10);
	rBCDYEAR = (set_time.year / 10) << 4 | (set_time.year % 10);
	rBCDMON = (set_time.mon / 10) << 4 | (set_time.mon % 10);
	rBCDDATE = (set_time.date / 10) << 4 | (set_time.date % 10);
	rBCDDAY = set_time.day;
	return 0;
}
ssize_t rtc_read(struct file * filp, char __user * buf, size_t size, loff_t * offp)
{
	struct TIME_RTC rtc_time;
	int len = sizeof(struct TIME_RTC);
	rtc_time.year = rBCDYEAR;
	rtc_time.year = (rtc_time.year >> 4) * 10 + (rtc_time.year & 0xf);
	rtc_time.mon = rBCDMON;
	rtc_time.mon = (rtc_time.mon >> 4) * 10 + (rtc_time.mon & 0xf);
	rtc_time.date = rBCDDATE;
	rtc_time.date = (rtc_time.date >> 4) * 10 + (rtc_time.date & 0xf);
	rtc_time.day = rBCDDAY;
	rtc_time.hour = rBCDHOUR;
	rtc_time.hour = (rtc_time.hour >> 4) * 10 + (rtc_time.hour & 0xf);
	rtc_time.min = rBCDMIN;
	rtc_time.min = (rtc_time.min >> 4) * 10 + (rtc_time.min & 0xf);
	rtc_time.sec = rBCDSEC;
	rtc_time.sec = (rtc_time.sec >> 4) * 10 + (rtc_time.sec & 0xf);
	/*Kernel space->user space */
	if (copy_to_user(buf, &rtc_time, len))	// pointer from the current position to start reading it + p 
	{
		printk("copy_to_user error \n");
		/*Bad address the wrong address */;
		return -EFAULT; 
	}
	return 0;
}

struct file_operations rtc_fops = { 
	.owner = THIS_MODULE, 
	.open = rtc_open, 
	.write = rtc_write, 
	.read = rtc_read, 
	.release = rtc_release, 
};

void rtc_cleanup_module(void) 
{
	 dev_t devno = MKDEV (rtc_MAJOR, rtc_MINOR); 
	 iounmap(rtc_base);
	iounmap(bcd_base);
	if (rtc_devices)
	 {
	cdev_del(&rtc_devices->cdev);	// 5 to remove a character device from the system
	kfree(rtc_devices);
	}
	unregister_chrdev_region(dev, 1);	// 6 release device number 
}


int rtc_init_module(void) 
{
	int result;
	 if (devmajor)
		 {
		dev = MKDEV(devmajor, devminor);	// 1 to obtain the device number 
		result = register_chrdev_region(dev, 1, "rtc_driver");	// 2 Assignment No. 
		}
	
	else
		 {
		result = alloc_chrdev_region(&dev, devminor, 1, "rtc_driver");
		
// // Dynamic allocation device number 2 
		    devmajor = MAJOR(dev);
		}
	if (result < 0)
		 {
		printk(KERN_WARNING "scull: can not get major%d \n",
			devmajor);
		return result;
		}
	printk(KERN_WARNING "led get major:%d \n", devmajor);
	rtc_devices = kmalloc(sizeof(struct rtc_dev), GFP_KERNEL);	// allocate memory for the device structure
	if (!rtc_devices)
		 {
		result = -ENOMEM;
		goto fail;
		}
	memset(rtc_devices, 0, sizeof(struct rtc_dev));
	cdev_init(&rtc_devices->cdev, &rtc_fops);	// (1) // character device registration, will structure embedded into their devices 
	rtc_devices->cdev.owner = THIS_MODULE;
	rtc_devices->cdev.ops = &rtc_fops;	// (2) 
	result = cdev_add(&rtc_devices->cdev, dev, 1);	// 4 ? the device to the kernel 
	if (result)
		 {
		printk(KERN_NOTICE "Error%d adding rtc \n", result);
		goto fail;
		}
	rtc_base = ioremap(0x57000040, 12);
	bcd_base = ioremap(0x57000070, 28);
	return 0;
 	fail:
		rtc_cleanup_module();// logout 
	return result;
}

module_init(rtc_init_module);	// Register 
module_exit(rtc_cleanup_module);	// logout 
MODULE_AUTHOR("hui");
MODULE_LICENSE("GPL");

