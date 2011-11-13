#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
/* System call stub. We initialize the stub function to be NULL.*/

//Syscall to start the ferry
long (*STUB_ferry_start)(void) = NULL;
EXPORT_SYMBOL(STUB_ferry_start);

//Syscall to send request
long (*STUB_ferry_request)(int passenger_request, char start_bank) = NULL;
EXPORT_SYMBOL(STUB_ferry_request);

//Syscall to stop ferry
long (*STUB_ferry_stop)(void) = NULL;
EXPORT_SYMBOL(STUB_ferry_stop);

/* System call wrapper. If the stub is not NULL, it will be run,
otherwise returns -ENOSYS */

asmlinkage long sys_ferry_start(void)
{
	if (STUB_ferry_start)
		return STUB_ferry_start();
	else
		return -ENOSYS;
}

asmlinkage long sys_ferry_request(int passenger_type, char start_bank)
{
	if (STUB_ferry_request)
		return STUB_ferry_request(passenger_type, start_bank);
	else
		return -ENOSYS;
}

asmlinkage long sys_ferry_stop(void)
{
	if (STUB_ferry_stop)
		return STUB_ferry_stop();
	else
		return -ENOSYS;
}
