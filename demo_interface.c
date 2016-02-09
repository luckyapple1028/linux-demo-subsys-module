/*
 * demo subsystem, interface functions
 *
*/

#include "demo.h"

int demo_test_set(struct demo_device *demo, struct demo_ctl_data *demo_ctl)
{
	int err = 0;

	err = mutex_lock_interruptible(&demo->ops_lock);
	if (err)
		return err;	

	if (demo->ops == NULL)
		err = -ENODEV;
	else if (!demo->ops->set_data)
		err = -EINVAL;
	else {
		/* do somerhing */
		demo->demo_data.text_data = demo_ctl->data;

		/* 调用驱动层接口 */
		err = demo->ops->set_data(demo->dev.parent, demo_ctl);
	}
	mutex_unlock(&demo->ops_lock);

	return err;	
}

int demo_test_get(struct demo_device *demo, struct demo_ctl_data *demo_ctl)
{
	int err = 0;

	err = mutex_lock_interruptible(&demo->ops_lock);
	if (err)
		return err;	

	if (demo->ops == NULL)
		err = -ENODEV;
	else if (!demo->ops->get_data)
		err = -EINVAL;
	else {
		/* do somerhing */
		demo_ctl->data = demo->demo_data.text_data;

		/* 调用驱动层接口 */
		err = demo->ops->get_data(demo->dev.parent, demo_ctl);
	}
	mutex_unlock(&demo->ops_lock);

	return err;	
}


