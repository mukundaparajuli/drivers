#include <linux/module.h>
#include <linux/fs.h>        // file_operations
#include <linux/cdev.h>      // cdev struct


// assigning a name to the device
#define DEVICE_NAME "mychar"
// initializing a major varable which is an unique identifier for the device so that kernel can identify it uniquely
static int major;
// Future use for managing advanced character device (e.g. with minor number support)
static struct cdev my_cdev;

// File operations
static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device open\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device close\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    printk(KERN_INFO "Read called\n");
    return 0;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    printk(KERN_INFO "Write called\n");
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

// runs when the device loads
// registers a device
static int __init mychar_init(void) {
    //register_chrdev(0, ...)` le kernel sanga request garcha to assign a **free major number**, ani yo driver lai bind garcha `file_operations` sanga.

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Register failed\n");
        return major;
    }
    printk(KERN_INFO "Registered with major number %d\n", major);
    return 0;
}

// os can only make the operations on the registered devices
// here all the functions defined above are the functions that os can run 


// unregisters a device
// runs when the driver exits
static void __exit mychar_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(mychar_init);
module_exit(mychar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mukunda");
MODULE_DESCRIPTION("Simple Character Device Driver");
