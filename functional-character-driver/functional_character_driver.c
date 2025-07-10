#include <asm-generic/errno-base.h>
#include <linux/module.h>
#include <linux/fs.h>        // file_operations
#include <linux/cdev.h>      // cdev struct


#define DEVICE_NAME "mychar"
#define BUFFER_SIZE 1024

static char kernel_buffer[BUFFER_SIZE];
static int data_size = 0;
static int major;
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

// *buf - user le provide gareko buffer to receive the data
// len - user le chaiyo bhaneko max byte
// *offset - file ko offset to track kati file read bhayo 
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {

    // yo condition = file already read bhaisakyo tesaile this represents end of file
    if(*offset >= data_size) return 0;

    if(len > data_size - *offset){
        len = data_size - *offset;
    }

    // copy_to_user le kernel buffer bata user space ma data laijane kaam garxa
    if(copy_to_user(buf, kernel_buffer+*offset, len)){
        // EFAULT = Error: Bad address (Faulty pointer)
        return -EFAULT;
    }

    *offset += len;
    printk(KERN_INFO "Sent %zu bytes to the user space", len);
    return len;
}


// *file => open bhayeko file ko pointer
// *buf => user space ko address - jun bata input aayeko data hamle write gardai xam
// len => kati byte of data input aayo tesko number
// offset => file ko offset

// ssize_t represents - kati data input aayo tesko lagi return type
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    // maximum buffer size bhanda dherai data aayo bhane trim garne
    // to avoid buffer overflow
    if(len > BUFFER_SIZE) len = BUFFER_SIZE;

    // user space bata kernal buffer ma data copy garxa
    if(copy_from_user(kernel_buffer, buf, len)){
        return -EFAULT;
    }

    data_size = len;

    printk(KERN_INFO "Received %zu bit from user space", len);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

static int __init mychar_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Register failed\n");
        return major;
    }
    printk(KERN_INFO "Registered with major number %d\n", major);
    return 0;
};

static void __exit mychar_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(mychar_init);
module_exit(mychar_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mukunda");
MODULE_DESCRIPTION("Simple Character Device Driver");
