/*
 * demo subsystem, sysfs interface
 *
*/

#include <linux/module.h>
#include "demo.h"
#include "demo-core.h"


/* device attributes */

static ssize_t
demo_name_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", to_demo_device(dev)->name);
}
static DEVICE_ATTR_RO(demo_name);


static ssize_t
demo_data_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%ld\n", to_demo_device(dev)->demo_data.text_data);
}

static ssize_t
demo_data_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct demo_device *demo = to_demo_device(dev);
	unsigned long val = simple_strtoul(buf, NULL, 0);

	if (val >= 4096 || val == 0)
		return -EINVAL;

	demo->demo_data.text_data = (unsigned long)val;

	return n;
}
static DEVICE_ATTR_RW(demo_data);


static struct attribute *demo_attrs[] = {
	&dev_attr_demo_name.attr,
	&dev_attr_demo_data.attr,
	NULL,
};
ATTRIBUTE_GROUPS(demo);






static ssize_t
demo_sysfs_show_demodata(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct demo_device *demo = to_demo_device(dev);
	ssize_t retval;

	/* 输出子系统通用设备数据 */
	retval = sprintf(buf, "%ld\n", demo->demo_data.text_data);

	return retval;
}

static ssize_t
demo_sysfs_set_demodata(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct demo_device *demo = to_demo_device(dev);
	struct demo_ctl_data demo_ctl;	
	unsigned long val = 0;
	ssize_t retval;

	val = simple_strtoul(buf, NULL, 0);

	if (val >= 4096 || val == 0)
		retval = -EINVAL;

	/* 调用interface接口写入驱动数据 */
	demo_ctl.data = (unsigned long)val;

	retval = demo_test_set(demo, &demo_ctl);

	return (retval < 0) ? retval : n;
}
static DEVICE_ATTR(demodata, S_IRUGO | S_IWUSR,
		demo_sysfs_show_demodata, demo_sysfs_set_demodata);

void demo_sysfs_add_device(struct demo_device *demo)
{
	int err;

	/* 条件判断 */
	/* do something */

	/* 为需要的设备创建一些特殊的 sys 节点 */
	err = device_create_file(&demo->dev, &dev_attr_demodata);
	if (err)
		dev_err(demo->dev.parent,
			"failed to create alarm attribute, %d\n", err);
}

void demo_sysfs_del_device(struct demo_device *demo)
{
	device_remove_file(&demo->dev, &dev_attr_demodata);
}

void __init demo_sysfs_init(struct class *demo_class)
{
	/* 绑定通用sys节点，在注册设备时会依次生成 */
	demo_class->dev_groups = demo_groups;
}







