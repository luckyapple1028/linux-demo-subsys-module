/*
 * xxx demo device
 *
*/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/io.h>

static void demo_device_release(struct device *dev);

/* demo platform设备 */
static struct platform_device demo0_device = {
	.name		= "xxx_demo_device",
	.id		    = 0,
	.dev		= {
		.release	= demo_device_release,
	}
};

static struct platform_device demo1_device = {
	.name		= "xxx_demo_device",
	.id		    = 1,
	.dev		= {
		.release	= demo_device_release,
	}
};

/* demo 设备释放 */
static void demo_device_release(struct device *dev)
{
	return;
}

/* demo 设备加载 */
static int __init demo_device_init(void)
{ 
	
	printk(KERN_INFO "xxx demo device register\n");
    platform_device_register(&demo0_device);
    platform_device_register(&demo1_device);	
    return 0;
}

/* demo 设备卸载 */
static void __exit demo_device_exit(void)
{
	printk(KERN_INFO "xxx demo device unregister\n");
    platform_device_unregister(&demo0_device);
	platform_device_unregister(&demo1_device);
}

module_init(demo_device_init);
module_exit(demo_device_exit);


MODULE_LICENSE("GPL"); 