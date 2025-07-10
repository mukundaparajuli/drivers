# 🛠️ Linux Character Device Driver - Full Concept Note

## 📌 What is a Linux Driver?

- Linux driver = code that runs in **kernel space** to control hardware or virtual devices.
- In Linux, **everything is a file**, so drivers expose devices as `/dev/...`.



## 🧾 What is a Character Driver?

- Character Driver handles devices that send/receive data as a **stream of bytes**.
- Examples: Keyboard, Mouse, Serial Ports, Virtual Devices



## 🔧 Tools Needed

| Tool | Why? |
|------|------|
| GCC + Make | Compile kernel modules |
| `linux-headers` | To get kernel structures & APIs |
| `insmod`, `rmmod`, `dmesg` | Manage and debug drivers |
| `mknod` | Manually create device files in `/dev` |



## 🗂️ Basic File Structure

```c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
```

* `module.h` → Basic kernel module macros
* `fs.h` → File operations struct
* `cdev.h` → `cdev` structure for character devices



## 📚 Key Components

| Concept                 | Explanation                                   |
| ----------------------- | --------------------------------------------- |
| `register_chrdev()`     | Register character device (get major number)  |
| `unregister_chrdev()`   | Unregister device on exit                     |
| `file_operations`       | Function pointers that define driver behavior |
| `copy_from_user()`      | Copy data from user space to kernel           |
| `copy_to_user()`        | Copy data from kernel to user space           |
| `MODULE_LICENSE("GPL")` | Avoid kernel taint warning                    |



## 📦 Basic Structure

```c
#define DEVICE_NAME "mychar"
static int major;
static char kernel_buffer[1024];
static int data_size = 0;
```

* `DEVICE_NAME` → Name shown in `/dev`
* `major` → Unique device identifier given by kernel
* `kernel_buffer` → Stores user data in kernel space



## 📲 File Operations

```c
struct file_operations fops = {
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .release = my_release,
    .owner = THIS_MODULE
};
```

* Tells kernel: "When user opens/reads/writes this device, call these functions."



## 🔁 Driver Lifecycle

### `module_init(mychar_init)`

* Called when module is inserted (`insmod`)
* Registers character driver with kernel

### `module_exit(mychar_exit)`

* Called when module is removed (`rmmod`)
* Unregisters the device



## 📥 my\_write()

```c
ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    if (len > BUFFER_SIZE) len = BUFFER_SIZE;

    if (copy_from_user(kernel_buffer, buf, len)) return -EFAULT;

    data_size = len;
    return len;
}
```

* `copy_from_user()` → gets data from user program
* `-EFAULT` → user memory invalid
* `data_size` → stores how much data was written


## 📤 my\_read()

```c
ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    if (*offset >= data_size) return 0;

    if (len > data_size - *offset)
        len = data_size - *offset;

    if (copy_to_user(buf, kernel_buffer + *offset, len)) return -EFAULT;

    *offset += len;
    return len;
}
```

* `offset` → tracks how much user has already read
* Prevents sending same data again
* `copy_to_user()` → sends data to user program



## 🧪 How to Test

### 1. Compile driver

```bash
make
```

### 2. Insert module

```bash
sudo insmod mychar.ko
dmesg | tail
```

### 3. Create device file

```bash
sudo mknod /dev/mychar c <major> 0
sudo chmod 666 /dev/mychar
```

### 4. Test from terminal

```bash
echo "hello mukunda" > /dev/mychar
cat /dev/mychar
```



## 🧠 Concepts Recap

| Term                        | Meaning                                    |
| --------------------------- | ------------------------------------------ |
| **Character Device**        | Byte stream device                         |
| **Major Number**            | Kernel's way to identify driver            |
| **Minor Number**            | Identify specific device if multiple exist |
| **`copy_to_user()`**        | Kernel → user                              |
| **`copy_from_user()`**      | User → kernel                              |
| **`-EFAULT`**               | Error code: memory access failed           |
| **`offset`**                | File pointer (like read cursor)            |
| **`MODULE_LICENSE("GPL")`** | Avoid kernel warning & respect license     |

