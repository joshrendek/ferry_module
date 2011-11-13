#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
/* System call stub. We initialize the stub function to be NULL.*/

long (*STUB_test_newsyscall)(int test_int) = NULL;
EXPORT_SYMBOL(STUB_test_newsyscall);

/* System call wrapper. If the stub is not NULL, it will be run,
otherwise returns -ENOSYS */

asmlinkage long sys_test_newsyscall(int test_int)
{
	if (STUB_test_newsyscall)
		return STUB_test_newsyscall(test_int);
	else
		return -ENOSYS;
}
