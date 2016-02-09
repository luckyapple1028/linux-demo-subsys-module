/*
 * demo subsystem, proc interface
 *
*/

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "demo.h"
#include "demo-core.h"

struct proc_dir_entry *demo_proc = NULL;

static int demo_proc_show(struct seq_file *seq, void *offset)
{
	int err = 0;
	struct demo_device *demo = seq->private;
	const struct demo_class_ops *ops = demo->ops;

	/* 输出需要的subsys proc信息 */
	seq_printf(seq, "demo_com_data\t: %ld\n", demo->demo_data.text_data);
	seq_printf(seq, "\n");

	/* 输出驱动层proc信息 */
	if (ops->proc)
		err = ops->proc(demo->dev.parent, seq);

	return err;
}

static int demo_proc_open(struct inode *inode, struct file *file)
{
	int ret;
	struct demo_device *demo = PDE_DATA(inode);

	if (!try_module_get(THIS_MODULE))
		return -ENODEV;

	ret = single_open(file, demo_proc_show, demo);
	if (ret)
		module_put(THIS_MODULE);
	return ret;
}

static int demo_proc_release(struct inode *inode, struct file *file)
{
	int res = single_release(inode, file);
	module_put(THIS_MODULE);
	return res;
}

static const struct file_operations demo_proc_fops = {
	.open		= demo_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= demo_proc_release,
};

void demo_proc_add_device(struct demo_device *demo)
{
	/* 为新注册的设备分配 proc */
	proc_create_data(dev_name(&demo->dev), 0, demo_proc, &demo_proc_fops, demo);
}

void demo_proc_del_device(struct demo_device *demo)
{
	/* 注销设备 proc */
	remove_proc_entry(dev_name(&demo->dev), demo_proc);
}

void __init demo_proc_init(void)
{
	/* 创建 demo proc 目录 */
	demo_proc = proc_mkdir("driver/demo", NULL);
}

void __exit demo_proc_exit(void)
{
	/* 删除 demo proc 目录 */
	if (NULL != demo_proc)
		proc_remove(demo_proc);
}





