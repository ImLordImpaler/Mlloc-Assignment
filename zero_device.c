/*
 * zero_device.c
 *
 * A simple Linux kernel module that implements a read-only
 * character device which behaves like /dev/zero.
 *
 * The device name is configurable via a kernel/module parameter:
 *   zero_device_name=<name>
 *
 * When read, the device returns an infinite stream of zero bytes.
 */

 #include <linux/module.h>  // Allows dynamically loading kernal infra modules 
 #include <linux/kernel.h> // Equivelent to printf
 #include <linux/fs.h>   //allows file operations like struct
 #include <linux/cdev.h> //attach this code to a device number.
 #include <linux/device.h> //
 #include <linux/uaccess.h>
 #include <linux/slab.h>
 
 static char *zero_device_name = NULL; // Will store device name 
 module_param(zero_device_name, charp, 0444);  
 MODULE_PARM_DESC(zero_device_name, "Name of the zero character device");
 
 static dev_t dev_number;
 static struct cdev zero_cdev;
 static struct class *zero_class;
 
 /*
  * Read callback: behaves like /dev/zero
  * - Fills the user buffer with zeros
  * - Always returns the requested byte count
  * - Never signals EOF
  */
 static ssize_t zero_read(struct file *file,
                          char __user *buf,
                          size_t count,
                          loff_t *offset)
 {
     char *kbuf;
 
     if (count == 0)
         return 0;
 
     kbuf = kzalloc(count, GFP_KERNEL);
     if (!kbuf)
         return -ENOMEM;
 
     if (copy_to_user(buf, kbuf, count)) {~
         kfree(kbuf);
         return -EFAULT;
     }
 
     kfree(kbuf);
     return count;
 }
 
 static const struct file_operations zero_fops = {
     .owner = THIS_MODULE,
     .read  = zero_read,
 };
 
 /*
  * Module initialization
  */
 static int __init zero_init(void)
 {
     int ret;
 
     if (!zero_device_name || zero_device_name[0] == '\0') {
         pr_err("zero_device: device name not specified\n");
         return -EINVAL;
     }
 
     /* Allocate a character device number */
     ret = alloc_chrdev_region(&dev_number, 0, 1, zero_device_name);
     if (ret < 0)
         return ret;
 
     /* Initialize and add cdev */
     cdev_init(&zero_cdev, &zero_fops);
     zero_cdev.owner = THIS_MODULE;
 
     ret = cdev_add(&zero_cdev, dev_number, 1);
     if (ret)
         goto err_unregister;
 
     /* Create sysfs class */
     zero_class = class_create("zero_device_class");
     if (IS_ERR(zero_class)) {
         ret = PTR_ERR(zero_class);
         goto err_cdev_del;
     }
 
     /* Create device node and sysfs entry */
     if (!device_create(zero_class, NULL, dev_number, NULL, zero_device_name)) {
         ret = -EINVAL;
         goto err_class_destroy;
     }
 
     pr_info("zero_device: loaded (/dev/%s)\n", zero_device_name);
     return 0;
 
 err_class_destroy:
     class_destroy(zero_class);
 err_cdev_del:
     cdev_del(&zero_cdev);
 err_unregister:
     unregister_chrdev_region(dev_number, 1);
     return ret;
 }
 
 /*
  * Module cleanup
  */
 static void __exit zero_exit(void)
 {
     device_destroy(zero_class, dev_number);
     class_destroy(zero_class);
     cdev_del(&zero_cdev);
     unregister_chrdev_region(dev_number, 1);
 
     pr_info("zero_device: unloaded\n");
 }
 
 module_init(zero_init);
 module_exit(zero_exit);
 
 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("Candidate");
 MODULE_DESCRIPTION("Custom zero character device (read-only)");
 