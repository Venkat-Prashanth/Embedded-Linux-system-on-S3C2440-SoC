
#define SUCCESS 0
#define DEV_NAME "myled"

#include <asm/ioctl.h>

#define LED_MAGIC 'L'
#define LED_ON		_IOW( LED_MAGIC, 1, uint )
#define LED_OFF		_IOW( LED_MAGIC, 2, uint )
#define LED_MAX_CMDS	2


