#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/bcd.h>
#include "led.h"
/* 
        As per mini2440 datasheet leds are connected to port B GPIO 

                LED     GPIO    Name in schematic
                ---------------------------------
                1       GPB5       nLED_1
                2       GPB6       nLED_2
                3       GPB7       nLED_3
                4       GPB8       nLED_4


        As per S3C2440 soc datasheet 
        ----------------------------

        PORT B CONTROL REGISTERS (GPBCON, GPBDAT, GPBUP)
        
        Register        Address         R/W     Description                                     Reset Value
        ---------------------------------------------------------------------------------------------------
        GPBCON          0x56000010      R/W     Configures the pins of port B                      0x0
        GPBDAT          0x56000014      R/W     The data register for port B                       undef
        GPBUP           0x56000018      R/W     Pull-up disable register for port B                0x0

        Config pins for nLED_4 
        ---------------------
        GPB8            [17:16]         01 = Output
        

        GPB Pull-up register
        --------------------

        GPBUP           Bit             Description
        -------------------------------------------
        GPB[10:0]       [10:0]          0: The pull up function attached to the corresponding port pin is enabled.
                                        1: The pull up function is disabled.
	GPB Data register
        -----------------
        GPBDAT          Bit             Description
        -------------------------------------------
        GPB[10:0]       [10:0]          When the port is configured as input port, the corresponding bit is the pin
                                        state. When the port is configured as output port, the pin state is the same
                                        as the corresponding bit.
        
*/

#define GPBCON          0x56000010
#define GPBDAT          0x56000014
#define GPBUP           0x56000018

void __iomem *config_reg, *data_reg, *up_reg;

static int led_open(struct inode *rtc_inode, struct file *file)
{
        pr_info("%s: led node opened\n",__func__);
        return 0;
}

static int led_close(struct inode *rtc_inode, struct file *file)
{
        pr_info("%s: led node closed\n",__func__);
        return 0;
}




static long  led_ioctl(struct file *filp,
                          unsigned int cmd,
                          unsigned long arg)
{
 /*     step 1: verify args */

        uint32_t size, data;

        if( _IOC_TYPE(cmd) != LED_MAGIC ) return -ENOTTY;
        if( _IOC_NR(cmd) > LED_MAX_CMDS ) return -ENOTTY;


        switch(cmd) {
        	case LED_ON:
			/* read GPB data register */
        		data = ioread32(data_reg);

        		/* on nLED_4 */
        		data = data & ~(1U<<8);
        		iowrite32(data, data_reg);
			break;

		case LED_OFF:
			/* read GPB data register */
        		data = ioread32(data_reg);
        		/* off nLED_4 */
        		data = data | (1U<<8);
        		iowrite32(data, data_reg);
                	break;
        }

        return SUCCESS;		
}

static struct file_operations led_ops = {
        .open = led_open,
        .release = led_close,
	.unlocked_ioctl = led_ioctl,
};

static struct miscdevice LedMisc = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = DEV_NAME,
        .fops = &led_ops,
};

static int __init led_init(void)
{
	unsigned int data;
        int ret;

        config_reg = ioremap(GPBCON, sizeof(int) * 3);
        data_reg = config_reg+4;
        up_reg = data_reg+4;

	/* read GPB configure register */
        data = ioread32(config_reg);

        /* Configure GPB8 as output pin 
	   
	   Config pins for nLED_4 
           ---------------------
           GPB8            [17:16]         01 = Output
	*/
        data = data | (1<<16);
        iowrite32(data, config_reg);

        /* read GPB pull-up  register */
        data = ioread32(up_reg);

        /* Enable GPB pull-up register 
		LED     GPIO    Name in schematic
                ---------------------------------
                4       GPB8       nLED_4

		GPBUP           Bit             Description
        	-------------------------------------------
        	GPB[10:0]       [10:0]          0: The pull up function attached to the corresponding port pin is enabled.
                	                        1: The pull up function is disabled.

	*/
        data = data & ~(1U<<8);
        iowrite32(data, up_reg);

	ret = misc_register(&LedMisc);
        if (ret < 0) {
                pr_err("%s: failed registering with minor %d",__func__, LedMisc.minor);
                return ret;
        }
        pr_info("%s: driver registered with Minor %d",__func__, LedMisc.minor);

        return SUCCESS;
}

static void __exit led_exit(void)
{
	unsigned int data;
	/* read GPB pull-up  register */
        data = ioread32(up_reg);

        /* Disable GPB pull-up register */
        data = data | (1U<<8);
        iowrite32(data, up_reg);

        iounmap(config_reg);
        misc_deregister(&LedMisc);

}

module_init(led_init);
module_exit(led_exit);

