#include <linux/module.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");

/* Extern system call stub declarations */

extern long (*STUB_test_newsyscall)(int test_int);

long my_test_newsyscall(int test)
{
	printk("%s: Your int is %i\n", __FUNCTION__, test);
	return test;
}

my_module_init() {
	STUB_test_newsyscall=&(my_test_newsyscall);
	return 0;
}

my_module_exit() {
	STUB_test_newsyscall=NULL;
}

module_init(my_module_init);
module_exit(my_module_exit);
