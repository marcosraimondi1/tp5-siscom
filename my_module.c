/**
 * @file gpio_driver.c
 * @brief GPIO driver for Linux kernel
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/workqueue.h>

/* Module information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Neuronas");
MODULE_DESCRIPTION("GPIO driver");

/* GPIO pins */
#define IO_A 538 /**< GPIO pin A */
#define IO_B 539 /**< GPIO pin B */

/* Globals */
static dev_t first;       /**< Global variable for the first device number */
static struct cdev c_dev; /**< Global variable for the character device structure */
static struct class *cl;  /**< Global variable for the device class */
static int chosen_pin = IO_A;          /**< Selected GPIO to read */
static struct delayed_work my_work;    /**< Delayed work structure */
static struct workqueue_struct *my_wq; /**< Workqueue structure */

static char last_value[8] = "0\n"; /**< Buffer to store the last read value */
static int read_period = HZ;       /**< 1 second delay */

/* ---------------- */
/* File operations */
/* ---------------- */

/**
 * @brief Release function for the device
 * @param inode Pointer to the inode structure
 * @param file Pointer to the file structure
 * @return 0 on success
 */
static int my_release(struct inode *inode, struct file *file) {
  printk("Closing my_module\n");
  return 0;
}

/**
 * @brief Open function for the device
 * @param i Pointer to the inode structure
 * @param f Pointer to the file structure
 * @return 0 on success
 */
static int my_open(struct inode *i, struct file *f) {
  printk("Opening my_module\n");
  return 0;
}

/**
 * @brief Read function for the device
 * @param file Pointer to the file structure
 * @param buf User buffer to store data
 * @param cnt Number of bytes to read
 * @param off Offset within the file
 * @return Number of bytes read on success, negative value on failure
 */
ssize_t my_read(struct file *file, char __user *buf, size_t cnt, loff_t *off) {
  int to_copy = cnt > 8 ? 8 : cnt;
  return to_copy - copy_to_user(buf, last_value, to_copy);
}

/**
 * @brief Write function for the device
 * @param file Pointer to the file structure
 * @param buf User buffer containing data to write
 * @param cnt Number of bytes to write
 * @param off Offset within the file
 * @return Number of bytes written on success, negative value on failure
 */
static ssize_t my_write(struct file *file, const char __user *buf, size_t cnt,
                        loff_t *off) {
  u8 val;

  if (cnt == 0)
    return 0;

  val = buf[0] - '0'; // convert ascii char to int

  if (val == 0) {
    chosen_pin = IO_A;
  } else {
    chosen_pin = IO_B;
  }

  printk("chosen GPIO: %d\n", chosen_pin - 512);

  return cnt;
}

/** File operations structure */
static struct file_operations my_fops = {
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release,
};

/**
 * @brief Work function for delayed work
 * @param work Pointer to the work structure
 */
static void work_function(struct work_struct *work) {
  u8 value;

  value = gpio_get_value(chosen_pin);
  snprintf(last_value, 8, "%d\n", value);

  // Queue the work again to run after 1 second
  queue_delayed_work(my_wq, &my_work, read_period);
}

/* ------------------- */
/* Lifecycle functions */
/* ------------------- */

/**
 * @brief Initialization function
 * @return 0 on success, negative value on failure
 */
int __init my_init(void) {
  printk("Hello World - GPIO\n");

  int status;
  int ret;
  struct device *dev_ret;

  // request GPIO pins

  status = gpio_request(IO_A, "IO_A");
  if (status != 0) {
    printk("Error requesting IO_A\n");
    return status;
  }

  status = gpio_request(IO_B, "IO_B");
  if (status != 0) {
    printk("Error requesting IO_B\n");
    gpio_free(IO_A);
    return status;
  }

  // set directions

  status = gpio_direction_input(IO_A);
  if (status != 0) {
    printk("Error setting direction input IO_A\n");
    gpio_free(IO_A);
    gpio_free(IO_B);
    return status;
  }

  status = gpio_direction_input(IO_B);
  if (status != 0) {
    printk("error setting direction input io_b\n");
    gpio_free(IO_A);
    gpio_free(IO_B);
    return status;
  }

  // register char device

  if ((ret = alloc_chrdev_region(&first, 0, 1, "my_module")) < 0) {
    gpio_free(IO_A);
    gpio_free(IO_B);
    return ret;
  }

  if (IS_ERR(cl = class_create(THIS_MODULE, "my_module"))) {
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return PTR_ERR(cl);
  }

  if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "my_module"))) {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return PTR_ERR(dev_ret);
  }

  cdev_init(&c_dev, &my_fops);
  if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return ret;
  }

  // Create workqueue
  my_wq = create_singlethread_workqueue("my_wq");
  if (!my_wq) {
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return -ENOMEM;
  }

  // Initialize delayed work
  INIT_DELAYED_WORK(&my_work, work_function);

  // Queue the first work
  queue_delayed_work(my_wq, &my_work, read_period);

  return 0;
}

/**
 * @brief Exit function
 */
void __exit my_exit(void) {
  printk("Goodbye! \n");

  // Cancel the delayed work and destroy the workqueue
  cancel_delayed_work_sync(&my_work);
  destroy_workqueue(my_wq);

  cdev_del(&c_dev);
  device_destroy(cl, first);
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
  gpio_free(IO_A);
  gpio_free(IO_B);
}

module_init(my_init);
module_exit(my_exit);
