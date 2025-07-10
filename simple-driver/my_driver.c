#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mukunda");
MODULE_DESCRIPTION("Simple Linux Driver");


// static indicates that this can run only inside this file - cannot be called from other files
// this function can return integer value
//  >= 0 - success
//  < 0 - failure

// linux-kernel macros
// __init - indicates that this function runs only when the driver loads
// __exit - indicates that this function runs only when the driver unloads

static int __init my_driver_init(void) {
    printk(KERN_INFO "Yo driver load bhaisakyo.\n");
    return 0;
}

static void __exit my_driver_exit(void) {
    printk(KERN_INFO "Driver unload bhayo.\n");
}



module_init(my_driver_init); // indicates which function to run on driver loading
module_exit(my_driver_exit); // indicates which function to run on driver unloading


// __init signals the compiler that this compiler is going to run just at the time of loading so optimise memory accordingly
// module_init tells the kernel what function to run when the driver loads