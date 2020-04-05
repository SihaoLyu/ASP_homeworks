/* **************** LDD:2.0 s_02/lab1_chrdrv.c **************** */
/*
 * The code herein is: Copyright Jerry Cooperstein, 2012
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/* 
Sample Character Driver 
@*/

#include <linux/module.h>	/* for modules */
#include <linux/fs.h>		/* file_operations */
#include <linux/uaccess.h>	/* copy_(to,from)_user */
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/cdev.h>		/* cdev utilities */
#include <linux/fcntl.h>
#include <linux/device.h>
#include <linux/kernel.h>


#define MYDEV_NAME "SihaoLyu"
#define MY_MAGIC 'A'
#define ASP_CLEAR_BUF _IO(MY_MAGIC, 0)


#define RAMDISK_SIZE (size_t) (16*PAGE_SIZE)


static const struct asp_mycdev {
	struct cdev dev;
	char *ramdisk;
	struct semaphore sem;
	int devNo;
    int disk_size;
};


static struct asp_mycdev* my_devices = NULL;
static unsigned int count = 3;
static int my_major = 0, my_minor = 0;
static struct class* my_class;


module_param(count, int, S_IRUGO);
module_param(my_major, int, S_IRUGO);


static int mycdrv_open(struct inode *inode, struct file *filp) {
    struct asp_mycdev* dev;
    dev = container_of(inode->i_cdev, struct asp_mycdev, dev);
    filp->private_data = dev;

	pr_info(" OPENING device: %s:\n\n", MYDEV_NAME);
	return 0;
}

static int mycdrv_release(struct inode *inode, struct file *filp) {
	pr_info(" CLOSING device: %s:\n\n", MYDEV_NAME);
	return 0;
}

static ssize_t
mycdrv_read(struct file *filp, char __user * buf, size_t lbuf, loff_t * ppos) {
	int nbytes;
    struct asp_mycdev* mydev = filp->private_data;
    down_interruptible(&mydev->sem);

	if ((lbuf + *ppos) > mydev->disk_size) {
		pr_info("trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_to_user(buf, mydev->ramdisk + *ppos, lbuf);

    up(&mydev->sem);
	*ppos += nbytes;
	pr_info("\n READING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static ssize_t
mycdrv_write(struct file *filp, const char __user * buf, size_t lbuf, loff_t * ppos) {
	int nbytes;
    struct asp_mycdev* mydev = filp->private_data;
    down_interruptible(&mydev->sem);

	if ((lbuf + *ppos) > mydev->disk_size) {
		pr_info("trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_from_user(mydev->ramdisk + *ppos, buf, lbuf);

    up(&mydev->sem);
	*ppos += nbytes;
	pr_info("\n WRITING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}


static loff_t mycdrv_llseek(struct file* filp, loff_t off, int whence) {
    loff_t newpos;
    struct asp_mycdev* mydev = filp->private_data;
    if (off < 0) { return -EINVAL; }

    switch (whence) {
        case SEEK_SET:
            newpos = off;
            break;
        
        case SEEK_CUR:
            newpos = filp->f_pos + off;
            break;

        case SEEK_END:
            newpos = mydev->disk_size + off;
            break;
        
        default:
            return -EINVAL;
    }

    if (newpos > mydev->disk_size) {
        down_interruptible(&mydev->sem);

        char* new_space = kmalloc(newpos, GFP_KERNEL);
        memset(new_space, 0, newpos);
        memcpy(new_space, mydev->ramdisk, mydev->disk_size);
        kfree(mydev->ramdisk);
        mydev->ramdisk = new_space;
        mydev->disk_size = newpos;

        up(&mydev->sem);
        
    }

    filp->f_pos = newpos;
    return newpos;
}


static long mycdrv_ioctl(struct file* filp, unsigned int cmd, unsigned long arg) {
    struct asp_mycdev* mydev = filp->private_data;

    switch (cmd) {
        case ASP_CLEAR_BUF: {
            down_interruptible(&mydev->sem);

            kfree(mydev->ramdisk);
            mydev->ramdisk = NULL;

            up(&mydev->sem);
            filp->f_pos = 0;
            break;
        }
        
        default:
            return -EINVAL;
    }

    return 0;
}


static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
    .llseek = mycdrv_llseek,
    .unlocked_ioctl = mycdrv_ioctl,
	.open = mycdrv_open,
	.release = mycdrv_release,
};


static void setup_cdev(struct asp_mycdev* my_dev, int index) {
    dev_t devno = MKDEV(my_major, my_minor + index);
    cdev_init(&my_dev->dev, &mycdrv_fops);
    my_dev->dev.owner = THIS_MODULE;
    my_dev->dev.ops = &mycdrv_fops;
    int err = cdev_add(&my_dev->dev, devno, 1);
    if (err) {
        printk(KERN_NOTICE "Error %d adding scull%d", err, index);
    }

    char buf[20];
    sprintf(buf, "mycdev%d", index);
    device_create(my_class, NULL, devno, NULL, buf);
}


static void my_cleanup_module(void) {
    dev_t devno = MKDEV(my_major, my_minor);

    if (my_devices) {
        int i;
        for (i=0; i<count; i++) {
            device_destroy(my_class, MKDEV(my_major, my_minor+i));
            kfree(my_devices[i].ramdisk);
            my_devices[i].ramdisk = NULL;
            cdev_del(&my_devices[i].dev);
        }
        
        class_destroy(my_class);
        kfree(my_devices);
        my_devices = NULL;
    }

    unregister_chrdev_region(devno, count);
}


static int __init my_init(void) {
    int res;
    dev_t dev = 0;

    if (my_major) {
        dev = MKDEV(my_major, my_minor);
        res = register_chrdev_region(dev, count, MYDEV_NAME);
    }
    else {
        res = alloc_chrdev_region(&dev, my_minor, count, MYDEV_NAME);
        my_major = MAJOR(dev);
    }

    if (res < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", my_major);
		return res;
    }

    my_devices = kmalloc(count * sizeof(struct asp_mycdev), GFP_KERNEL);
    if (!my_devices) { 
        my_cleanup_module();
        res = -ENOMEM; 
        return res;
    }
    memset(my_devices, 0, count * sizeof(struct asp_mycdev));

    my_class = class_create(THIS_MODULE, MYDEV_NAME);

    int i;
    for (i=0; i<count; i++) {
        my_devices[i].ramdisk = kmalloc(RAMDISK_SIZE, GFP_KERNEL);
        my_devices[i].devNo = i;
        my_devices[i].disk_size = RAMDISK_SIZE;
        sema_init(&my_devices[i].sem, 1);
        setup_cdev(&my_devices[i], i);
    }

    pr_info("Succeeded in registering character device %s\n", MYDEV_NAME);
    return 0;
}


static void __exit my_exit(void) {
    my_cleanup_module();
}


module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sihao Lyu");
MODULE_LICENSE("GPL v2");
