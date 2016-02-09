/*
 * core demo interface.
 *
*/

extern void __init demo_dev_init(void);
extern void __exit demo_dev_exit(void);
extern void demo_dev_prepare(struct demo_device *demo);
extern void demo_dev_add_device(struct demo_device *demo);
extern void demo_dev_del_device(struct demo_device *demo);

extern void __init demo_proc_init(void);
extern void __exit demo_proc_exit(void);
extern void demo_proc_add_device(struct demo_device *demo);
extern void demo_proc_del_device(struct demo_device *demo);

extern void __init demo_sysfs_init(struct class *);
extern void demo_sysfs_add_device(struct demo_device *demo);
extern void demo_sysfs_del_device(struct demo_device *demo);

