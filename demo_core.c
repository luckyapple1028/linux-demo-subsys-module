/*
 * demo subsystem, base core
 *
*/

#include <linux/module.h>
#include <linux/of.h>
#include <linux/rtc.h>
#include <linux/kdev_t.h>
#include <linux/idr.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include "demo.h"
#include "demo-core.h"


static DEFINE_IDA(demo_ida);
struct class *demo_class;

static void demo_device_release(struct device *dev)
{
	struct demo_device *demo = to_demo_device(dev);
	ida_simple_remove(&demo_ida, demo->id);
	kfree(demo);
}

struct demo_device *demo_device_register(const char *name, struct device *dev,
					const struct demo_class_ops *ops,
					struct module *owner)
{
	struct demo_device *demo;
	int of_id = -1, id = -1, err;

	/* 获取ID号 */
	if (dev->of_node)
		of_id = of_alias_get_id(dev->of_node, "demo");
	else if (dev->parent && dev->parent->of_node)
		of_id = of_alias_get_id(dev->parent->of_node, "demo");

	if (of_id >= 0) {
		id = ida_simple_get(&demo_ida, of_id, of_id + 1, GFP_KERNEL);
		if (id < 0)
			dev_warn(dev, "/aliases ID %d not available\n", of_id);
	}

	if (id < 0) {
		id = ida_simple_get(&demo_ida, 0, 0, GFP_KERNEL);
		if (id < 0) {
			err = id;
			goto exit;
		}
	}

	/* 开始分配内存 */
	demo = kzalloc(sizeof(struct demo_device), GFP_KERNEL);
	if (demo == NULL) {
		err = -ENOMEM;
		goto exit_ida;
	}

	/* demo 结构初始化 */
	demo->id = id;
	demo->ops = ops;
	demo->owner = owner;
	demo->dev.parent = dev;
	demo->dev.class = demo_class;
	demo->dev.release = demo_device_release;

	mutex_init(&demo->ops_lock);
	spin_lock_init(&demo->irq_lock);
	init_waitqueue_head(&demo->irq_queue);

	strlcpy(demo->name, name, DEMO_DEVICE_NAME_SIZE);
	dev_set_name(&demo->dev, "demo%d", id);

	/* 字符设备初始化 */
	demo_dev_prepare(demo);

	err = device_register(&demo->dev);
	if (err) {
		put_device(&demo->dev);
		goto exit_kfree;
	}

	/* 字符设备、sysfs设备和proc设备注册添加 */
	demo_dev_add_device(demo);
	demo_sysfs_add_device(demo);
	demo_proc_add_device(demo);

	dev_notice(dev, "demo core: registered %s as %s\n", demo->name, dev_name(&demo->dev));

	return demo;

exit_kfree:
	kfree(demo);

exit_ida:
	ida_simple_remove(&demo_ida, id);

exit:
	dev_err(dev, "demo core: unable to register %s, err = %d\n", name, err);
	return ERR_PTR(err);
}
EXPORT_SYMBOL_GPL(demo_device_register);


void demo_device_unregister(struct demo_device *demo)
{
	if (get_device(&demo->dev) != NULL) {
		mutex_lock(&demo->ops_lock);
		demo_sysfs_del_device(demo);
		demo_dev_del_device(demo);
		demo_proc_del_device(demo);
		device_unregister(&demo->dev);
		demo->ops = NULL;
		mutex_unlock(&demo->ops_lock);
		put_device(&demo->dev);
	}
}
EXPORT_SYMBOL_GPL(demo_device_unregister);


static void devm_demo_device_release(struct device *dev, void *res)
{
	struct demo_device *demo = *(struct demo_device **)res;

	demo_device_unregister(demo);
}


static int devm_demo_device_match(struct device *dev, void *res, void *data)
{
	struct demo_device **r = res;

	return *r == data;
}


struct demo_device *devm_demo_device_register(struct device *dev,
					const char *name,
					const struct demo_class_ops *ops,
					struct module *owner)
{
	struct demo_device **ptr, *demo;

	ptr = devres_alloc(devm_demo_device_release, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return ERR_PTR(-ENOMEM);

	/* 注册 demo 设备 */
	demo = demo_device_register(name, dev, ops, owner);
	if (!IS_ERR(demo)) {
		*ptr = demo;
		devres_add(dev, ptr);
	} else {
		devres_free(ptr);
	}

	return demo;
}
EXPORT_SYMBOL_GPL(devm_demo_device_register);


void devm_demo_device_unregister(struct device *dev, struct demo_device *demo)
{
	int res;

	/* 注销 demo 设备 */
	res = devres_release(dev, devm_demo_device_release,
				devm_demo_device_match, demo);
	
	WARN_ON(res);
}
EXPORT_SYMBOL_GPL(devm_demo_device_unregister);


static int __init demo_core_init(void)
{ 
	/* 创建 demo class */
	demo_class = class_create(THIS_MODULE, "demo");
	if (IS_ERR(demo_class)) {
		pr_err("couldn't create class\n");
		return PTR_ERR(demo_class);
	}

	/* demo 设备驱动初始化 */
	demo_dev_init();

	/* demo proc初始化 */
	demo_proc_init();
	
	/* demo sysfs初始化 */
	demo_sysfs_init(demo_class);

	pr_info("demo subsys init success\n");	
	return 0;
}


static void __exit demo_core_exit(void)
{
	demo_proc_exit();
	demo_dev_exit();
	class_destroy(demo_class);
	ida_destroy(&demo_ida);
	
	pr_info("demo subsys exit success\n");
}

module_init(demo_core_init);
module_exit(demo_core_exit);

MODULE_LICENSE("GPL");  
MODULE_AUTHOR("zhangyi");