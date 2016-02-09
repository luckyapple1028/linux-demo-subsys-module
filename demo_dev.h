/*
 * Generic demo dev interface.
 *
*/

#ifndef _DEMO_DEV_H_
#define _DEMO_DEV_H_

struct demo_ctl_data
{
	unsigned long data;
};

#define DEMO_IOCTL_SET	_IOW('p', 0x01, struct demo_ctl_data) /* Set demo data  */
#define DEMO_IOCTL_GET	_IOR('p', 0x02, struct demo_ctl_data) /* Read demo data */


#endif /* _DEMO_DEV_H_ */



