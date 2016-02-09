/*
 * xxx demo driver
 *
*/


#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kmod.h>
#include <linux/timer.h>
#include <linux/of_platform.h>

#include "demo.h"

struct xxx_demo {
	struct demo_device *demo;
	struct timer_list xxx_demo_timer;
	unsigned long xxx_demo_data;
};




static int xxx_demo_open(struct device *dev)
{
	struct xxx_demo *xxx_demo = dev_get_drvdata(dev);
	
	printk(KERN_INFO "xxx demo open\n");

	/* do something open */
	(void)xxx_demo;
	
	return 0;
}

static void xxx_demo_release(struct device *dev)
{
	struct xxx_demo *xxx_demo = dev_get_drvdata(dev);

	printk(KERN_INFO "xxx demo release\n");

	/* do something close */
	(void)xxx_demo;
	
	return;
}

static int xxx_demo_ioctl(struct device *dev, unsigned int cmd, unsigned long arg)
{
	struct xxx_demo *xxx_demo = dev_get_drvdata(dev);
	
	printk(KERN_INFO "xxx demo ioctl\n");

	/* do something ioctl */
	(void)xxx_demo;
	
	return 0;
}

static int xxx_demo_set_data(struct device *dev, struct demo_ctl_data *ctrl_data)
{
	struct xxx_demo *xxx_demo = dev_get_drvdata(dev);
	
	printk(KERN_INFO "xxx demo set data\n");

	xxx_demo->xxx_demo_data = ctrl_data->data;
	return 0;
}

static int xxx_demo_get_data(struct device *dev, struct demo_ctl_data *ctrl_data)
{
	struct xxx_demo *xxx_demo = dev_get_drvdata(dev);
	
	printk(KERN_INFO "xxx demo get data\n");

	ctrl_data->data = xxx_demo->xxx_demo_data;
	return 0;
}

static int xxx_demo_proc(struct device *dev, struct seq_file *seq)
{
	struct xxx_demo *xxx_demo = dev_get_drvdata(dev);

	seq_printf(seq, "xxx_demo_data\t: %ld\n", xxx_demo->xxx_demo_data);
	seq_printf(seq, "\n");

	return 0;
}

static int xxx_demo_read (struct device *dev, int data)
{
	struct xxx_demo *xxx_demo = dev_get_drvdata(dev);

	printk(KERN_INFO "xxx demo read\n");
	return xxx_demo->xxx_demo_data;
}


struct demo_class_ops xxx_demo_ops = {
	.open		= xxx_demo_open,
	.release	= xxx_demo_release,
	.ioctl		= xxx_demo_ioctl,
	.set_data	= xxx_demo_set_data,
	.get_data	= xxx_demo_get_data,
	.proc		= xxx_demo_proc,
	.read_callback	= xxx_demo_read,
};



static void xxx_demo_time(unsigned long para)
{
	struct xxx_demo *xxx_demo = (struct xxx_demo *)para;

	xxx_demo->xxx_demo_data++;
	xxx_demo->demo->irq_data = 1;

	mod_timer(&xxx_demo->xxx_demo_timer, jiffies + HZ);
}

static int xxx_demo_driver_probe(struct platform_device *pdev)
{
	struct xxx_demo *xxx_demo = NULL;
	int ret = 0;
	
	/* 申请驱动结构内存并保存为platform的私有数据 */
	xxx_demo = devm_kzalloc(&pdev->dev, sizeof(struct xxx_demo), GFP_KERNEL);
	if (!xxx_demo)
		return -ENOMEM;

	platform_set_drvdata(pdev, xxx_demo);

	/* 获取平台资源 */
	/* do something */

	
	/* 执行驱动相关初始化(包括外设硬件、锁、队列等)*/
	xxx_demo->xxx_demo_data = 0;
	/* do something */
	init_timer(&xxx_demo->xxx_demo_timer);
	xxx_demo->xxx_demo_timer.function = xxx_demo_time;
	xxx_demo->xxx_demo_timer.data = (unsigned long)xxx_demo;
	xxx_demo->xxx_demo_timer.expires = jiffies + HZ;
	add_timer(&xxx_demo->xxx_demo_timer);

	/* 向 demo 子系统注册设备 */
	xxx_demo->demo = devm_demo_device_register(&pdev->dev, "xxx_demo",
				&xxx_demo_ops, THIS_MODULE);
	if (IS_ERR(xxx_demo->demo)) {
		dev_err(&pdev->dev, "unable to register the demo class device\n");
		ret = PTR_ERR(xxx_demo->demo);
		goto err;
	}
	
	return 0;

err:
	del_timer_sync(&xxx_demo->xxx_demo_timer);
	return ret;
}


static int xxx_demo_driver_remove(struct platform_device *pdev)
{
	struct xxx_demo *xxx_demo = platform_get_drvdata(pdev);

	/* 执行驱动相关去初始化(包括外设硬件、锁、队列等)*/
	del_timer_sync(&xxx_demo->xxx_demo_timer);	
	/* do something */

	/* 向 demo 子系统注销设备 */
	devm_demo_device_unregister(&pdev->dev, xxx_demo->demo);

	/* 释放驱动结构内存 */
	devm_kfree(&pdev->dev, xxx_demo);
	
	return 0;
}


static struct platform_driver xxx_demo_driver = {
	.driver	= {
		.name    = "xxx_demo_device",   
		.owner	 = THIS_MODULE,
	},
	.probe   = xxx_demo_driver_probe,
	.remove  = xxx_demo_driver_remove,
};


static int __init xxx_demo_driver_init(void)
{ 	
	printk(KERN_INFO "xxx demo driver register\n");
    platform_driver_register(&xxx_demo_driver);
	
    return 0;
}


static void __exit xxx_demo_driver_exit(void)
{	
	printk(KERN_INFO "xxx demo driver unregister\n");
    platform_driver_unregister(&xxx_demo_driver);
}


module_init(xxx_demo_driver_init);
module_exit(xxx_demo_driver_exit);


MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("zhangyi");
MODULE_ALIAS("platform:xxx_demo_device");
