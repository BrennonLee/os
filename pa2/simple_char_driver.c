
#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<asm/uaccess.h>

#define BUFFER_SIZE 1024

/* Define device_buffer and other global data structures you will need here */
static int openCounter = 0;
static int closeCounter = 0;
void *device_buffer;
void *buffer_start;

ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	//*device_buffer = *offset

	int str_length = strlen(device_buffer);
	if (length > BUFFER_SIZE){
		printk(KERN_ALERT "Bytes read: %i", BUFFER_SIZE);
		copy_to_user(buffer, device_buffer, BUFFER_SIZE);
		return 0;	
	}
	else if (str_length == 0){
		printk(KERN_ALERT "device_buffer is empty");
		return 0;	
	}
	else{
		copy_to_user(buffer, device_buffer, length);
		printk(KERN_ALERT "Bytes read: %i", length);
		return 0;
	}
	
}



ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */

	*offset = strlen(device_buffer);
	device_buffer = buffer_start;

	int remainingBuff = BUFFER_SIZE - *offset;

	if (*offset >= BUFFER_SIZE){
		printk(KERN_ALERT "Buffer is full, cannnot write.");
		return 0;
	}

	else if (length > remainingBuff){
		copy_from_user(device_buffer + *offset, buffer, remainingBuff);
		device_buffer += strlen(device_buffer);
		printk(KERN_ALERT "Ran out of space!");
		return remainingBuff;
	}

	else {
		copy_from_user(device_buffer + *offset, buffer, length);
		printk(KERN_ALERT "Bytes written: %d", strlen(device_buffer) - *offset);
		device_buffer += strlen(device_buffer);
		return length;
	}
}


int simple_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	openCounter++;	
	printk(KERN_ALERT "The device has been opened %i times.", openCounter);	
	return 0;
}

int simple_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	closeCounter++;	
	printk(KERN_ALERT "The device has been closed %i times.", closeCounter);	
	return 0;
}

loff_t simple_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	switch(whence){
	    case 0:
		/* SEEK_SET */
		/* Current position is set to offset value */
		printk(KERN_ALERT "SEEK_SET: old position: %i", device_buffer);
		device_buffer = buffer_start + offset;
		printk(KERN_ALERT "new position: %i", device_buffer);
		return device_buffer;
	    case 1:
		/* SEEK_CUR */
		/* Current position is incremented by the offset bytes */
		printk(KERN_ALERT "SEEK_CUR old position: %i", device_buffer);
		device_buffer += offset;
		printk(KERN_ALERT "new position: %i", device_buffer);
		return device_buffer;
	    case 2:
		/* SEEK_END */
		/* Current position is set to offset bytes before end of the file  */
		printk(KERN_ALERT "SEEK_END old position: %i", device_buffer);
		void *tmp = buffer_start + strlen(device_buffer);
		device_buffer = (tmp - offset);
		printk(KERN_ALERT "new position: %i", device_buffer);
		return device_buffer;
	    default:
		return device_buffer;
	}
}

struct file_operations simple_char_driver_file_operations = {

	.owner  = THIS_MODULE,
	.read	= simple_char_driver_read,
	.write	= simple_char_driver_write,
	.open	= simple_char_driver_open,
	.release = simple_char_driver_close,
	.llseek	= simple_char_driver_seek
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};

static int simple_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	register_chrdev(333, "simple_char_driver", &simple_char_driver_file_operations);
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	buffer_start = device_buffer;
	return 0;
}

static void simple_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	kfree(device_buffer);
	unregister_chrdev(333, "simple_char_driver");	
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);


