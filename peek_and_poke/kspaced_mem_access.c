#include <linux/device.h>
#include <linux/kernel.h>  /* We're doing kernel work */
#include <linux/kobject.h> /* Necessary because we use sysfs */
#include <linux/module.h>  /* Specifically, a module */
#include <asm/io.h>

#define sysfs_dir "peek_and_poke"
#define sysfs_input "input"
#define sysfs_output "output"
/* due to limitations of sysfs, you mustn't go above PAGE_SIZE, 1k is already a
 * *lot* of information for sysfs! */
#define sysfs_max_data_size 512

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Thimo Ferede");
MODULE_DESCRIPTION("peek and poke");

static char input_buffer[sysfs_max_data_size + 1]; /* an extra byte for the '\0' terminator */
static char output_buffer[sysfs_max_data_size + 1]; /* an extra byte for the '\0' terminator */
static ssize_t used_buffer_size = 0;

static ssize_t input_show(struct device *dev, struct device_attribute *attr,
        char *buffer)
{
  printk(KERN_INFO "input_read (/sys/kernel/%s/%s) called\n", sysfs_dir,
         sysfs_input);

  return sprintf(buffer, "%s", input_buffer);
}

static ssize_t input_store(struct device *dev, struct device_attribute *attr,
         const char *buffer, size_t count)
{
  used_buffer_size =
      count > sysfs_max_data_size
    ? sysfs_max_data_size
    : count; /* handle MIN(used_buffer_size, count) bytes */

  printk(KERN_INFO "input_write (/sys/kernel/%s/%s) called, buffer: "
       "%s, count: %u\n", sysfs_dir, sysfs_input, buffer, count);

  memcpy(input_buffer, buffer, used_buffer_size);
  input_buffer[used_buffer_size] = '\0';
  
  if (used_buffer_size == 0){
    printk(KERN_INFO "No input given");
    return 0;
  }

  // Create an object to hold the value of the buffer
  // This is necessary because the buffer is const
  // and some actions with it are not permitted
  char temp[sysfs_max_data_size + 1];
  strcpy((char*)temp, input_buffer);
  char* bufferPtr = temp;
  
  // Store the command arguments
  char args[3][64];

  // Fetch the arguments
  // Using while since for doesn't want to compile
  int i = 0;
  while (i < 3)
  {
    // Fetch argument i
    char* arg = strsep(&bufferPtr, " ");

    if (arg == NULL){
      printk(KERN_INFO "Command is missing arguments\n");
      return used_buffer_size;
    }

    // Safe argument
    strcpy(args[i], arg);
    i++;
  }

  // Convert address to the appropriate type
  // make it unsigned because it is an address
  char* address_str = args[1];
  u_long addr;
  kstrtol(address_str, 0, &addr);
  char* addrPtr = (char*)addr;

  if (args[0][0] == 'r'){
    char* count_str = args[2];

    // Convert the entered number of byte
    // into the correct format
    long nr_bytes;
    kstrtol(count_str, 0, &nr_bytes);
    
    printk(KERN_INFO "Reading %ld bytes from address %ld\n", nr_bytes, addr);

    // Fetch the data from the address
    i = 0;
    while (i < nr_bytes)
    {
        output_buffer[i] = *(addrPtr + i);
        i++;
    }
    
    // Mark end
    output_buffer[nr_bytes] = '\0';

    // Print debug info
    printk(
        KERN_INFO
        "retrieved data: %s\n",
        output_buffer
    );
  }
  else if (args[0][0] == 'w'){
    char* data = args[2];
    
    printk(KERN_INFO "Writing %s to address %ld\n", data, addr);
    
    // Write data
    i = 0;
    while (data[i] != '\0')
    {
        *(addrPtr + i) = data[i];
        i++;
    }
  }

  return used_buffer_size;
}

static ssize_t output_show(struct device *dev, struct device_attribute *attr,
        char *buffer)
{
  printk(KERN_INFO "output_read (/sys/kernel/%s/%s) called\n", sysfs_dir,
         sysfs_output);
  
  return sprintf(buffer, output_buffer);
}

static DEVICE_ATTR(input, S_IWUGO | S_IRUGO, input_show, input_store);
static DEVICE_ATTR(output, S_IRUGO, output_show, NULL);

static struct attribute *attrs[] = {
    &dev_attr_input.attr,
    &dev_attr_output.attr,
    NULL /* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *data_obj = NULL;

int __init sysfs_init(void)
{
  int result = 0;

  data_obj = kobject_create_and_add(sysfs_dir, kernel_kobj);
  if (data_obj == NULL) {
    printk(KERN_WARNING
        "module failed to load: kobject_create_and_add failed\n");
    return -ENOMEM;
  }

  result = sysfs_create_group(data_obj, &attr_group);
  if (result != 0) {
    printk(KERN_WARNING "module failed to load: sysfs_create_group "
         "failed with result %d\n", result);
    kobject_put(data_obj);
    return -ENOMEM;
  }

  printk(KERN_INFO "/sys/kernel/%s created\n", sysfs_dir);
  return result;
}

void __exit sysfs_exit(void)
{
  kobject_put(data_obj);
  printk(KERN_INFO "/sys/kernel/%s removed\n", sysfs_dir);
}

module_init(sysfs_init);
module_exit(sysfs_exit);
