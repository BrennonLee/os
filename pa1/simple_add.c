#include <linux/kernel.h>
#include <linux/linkage.h>
#include <asm/uaccess.h>

asmlinkage long sys_simple_add(int number1, int number2, int* result){
	
	int tmp;
	printk("Your first number is: %d\n", number1);
	printk("Your second number is: %d\n", number2);
	tmp = number1 + number2;
	copy_to_user(result, &tmp, sizeof(int));
	result = &tmp;
	printk("Your result is: %d\n", *result);
	return 0;
}
