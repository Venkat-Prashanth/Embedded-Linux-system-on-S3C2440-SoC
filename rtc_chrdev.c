#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/bcd.h>
#include "rtc.h"

void __iomem *base;
static int get_rtc(unsigned int addr)
{
	int data;
	data = readb(base + addr);
	return data;
}

static void set_rtc(unsigned char data, unsigned int addr)
{
	writeb(0x1, base + S3C2410_RTCCON);
	writeb(data, base + addr);
	writeb(0x0, base + S3C2410_RTCCON);

}

static int rtc_open(struct inode *rtc_inode, struct file *file)
{
	pr_info("%s: rtc node opened\n",__func__);
	return 0;
}

static int rtc_close(struct inode *rtc_inode, struct file *file)
{
	pr_info("%s: rtc node closed\n",__func__);
	return 0;
}


static ssize_t
rtc_read(struct file *file, char __user * buf, size_t cnt, loff_t * f_pos)
{
	unsigned int ret;
	struct rtc_time time = { 0 };

	if (cnt != sizeof(struct rtc_time)) {
		pr_err("invalid request");
		return -EINVAL;
	}
	pr_info("%s: rtc read invoked\n",__func__);


	writeb(0x1, base + S3C2410_RTCCON);

	time.sec = get_rtc(S3C2410_RTCSEC);
	time.min = get_rtc(S3C2410_RTCMIN);
	time.hour = get_rtc(S3C2410_RTCHOUR);
	time.day = get_rtc(S3C2410_RTCDATE);
	time.mon = get_rtc(S3C2410_RTCMON);
	time.year = get_rtc(S3C2410_RTCYEAR);

	time.sec = bcd2bin(time.sec & 0x7F);
	time.min = bcd2bin(time.min & 0x7F);
	time.hour = bcd2bin(time.hour);
	time.day = bcd2bin(time.day & 0x3F);
	time.mon = bcd2bin(time.mon & 0x1F);
	time.year = bcd2bin(time.year);
	writeb(0x0, base + S3C2410_RTCCON);

	ret = copy_to_user(buf, &time, sizeof(time));

	return cnt;
}


static ssize_t
rtc_write(struct file *file, const char __user * buf, size_t cnt,
	  loff_t * f_pos)
{

	unsigned int ret;
	 struct rtc_time time = { 0 };

        if (cnt != sizeof(struct rtc_time)) {
                pr_err("invalid request");
                return -EINVAL;
        }

	ret = copy_from_user(&time, buf, sizeof(time));

	pr_info("%s: set time %04d.%02d.%02d %02d:%02d:%02d\n",__func__,
                 1900 + time.year, time.mon, time.day,
                 time.hour, time.min, time.sec);


	writeb(0x1, base + S3C2410_RTCCON);

	writeb(bin2bcd(time.sec), base + S3C2410_RTCSEC);
        writeb(bin2bcd(time.min), base + S3C2410_RTCMIN);
        writeb(bin2bcd(time.hour),base + S3C2410_RTCHOUR);
        writeb(bin2bcd(time.day), base + S3C2410_RTCDATE);
        writeb(bin2bcd(time.mon), base + S3C2410_RTCMON);
        writeb(bin2bcd(time.year),base + S3C2410_RTCYEAR);

	writeb(0x0, base + S3C2410_RTCCON);

/*
        set_rtc(time.sec, S3C2410_RTCSEC);
        set_rtc(time.min, S3C2410_RTCMIN);
        set_rtc(time.hour,S3C2410_RTCHOUR);
        set_rtc(time.day, S3C2410_RTCDATE);
        set_rtc(time.mon, S3C2410_RTCMON);
        set_rtc(time.year,S3C2410_RTCYEAR );
*/
	return cnt;
}


static struct file_operations rtc_ops = {
	.open = rtc_open,
	.release = rtc_close,
	.read = rtc_read,
	.write = rtc_write,
};

static struct miscdevice RtcMisc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEV_NAME,
	.fops = &rtc_ops,
};

static int __init rtc_init(void)
{
	int ret;
	base = ioremap(0x57000000, 0x90);	
	ret = misc_register(&RtcMisc);
	if (ret < 0) {
		pr_err("%s: failed registering with minor %d",__func__, RtcMisc.minor);
		return ret;
	}
	pr_info("%s: driver registered with Minor %d",__func__, RtcMisc.minor);

	return SUCCESS;
}

static void __exit rtc_exit(void)
{
	iounmap(base);
	misc_deregister(&RtcMisc);

}

module_init(rtc_init);
module_exit(rtc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("support@techveda.org");
MODULE_DESCRIPTION("LKD_CW: sample char driver for cmos realtime clock");
