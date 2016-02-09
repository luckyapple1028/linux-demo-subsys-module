/*
 * Generic demo interface.
 *
*/
#ifndef _DEMO_MODULE_H_
#define _DEMO_MODULE_H_

#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/timerqueue.h>
#include <linux/workqueue.h>

#include "demo_dev.h"

#define DEMO_DEVICE_NAME_SIZE 20
#define DEMO_DEV_BUSY 0

struct demo_data
{
	unsigned long text_data;
};

struct demo_device
{
	struct device dev;
	struct module *owner;

	int id;
	char name[DEMO_DEVICE_NAME_SIZE];

	const struct demo_class_ops *ops;
	struct mutex ops_lock;

	struct cdev char_dev;
	unsigned long flags;

	unsigned long irq_data;
	spinlock_t irq_lock;
	wait_queue_head_t irq_queue;
	struct fasync_struct *async_queue;

	/* some demo data */
	struct demo_data demo_data;
};

#define to_demo_device(d) container_of(d, struct demo_device, dev)

struct demo_class_ops {
	int (*open)(struct device *);
	void (*release)(struct device *);
	int (*ioctl)(struct device *, unsigned int, unsigned long);
	int (*set_data)(struct device *, struct demo_ctl_data *);
	int (*get_data)(struct device *, struct demo_ctl_data *);
	int (*proc)(struct device *, struct seq_file *);
	int (*read_callback)(struct device *, int data);
};


extern struct demo_device *demo_device_register(const char *name,
					struct device *dev,
					const struct demo_class_ops *ops,
					struct module *owner);
extern struct demo_device *devm_demo_device_register(struct device *dev,
					const char *name,
					const struct demo_class_ops *ops,
					struct module *owner);
extern void demo_device_unregister(struct demo_device *demo);
extern void devm_demo_device_unregister(struct device *dev,
					struct demo_device *demo);

extern int demo_test_set(struct demo_device *demo, struct demo_ctl_data *demo_ctl);
extern int demo_test_get(struct demo_device *demo, struct demo_ctl_data *demo_ctl);


#endif /* _DEMO_MODULE_H_ */

