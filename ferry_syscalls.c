#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/module.h>
/* System call stub. We initialize the stub function to be NULL.*/

//Syscall to start the ferry
long (*STUB_ferry_start)(int start_int) = NULL;
EXPORT_SYMBOL(STUB_ferry_start);

//Syscall to send request
long (*STUB_ferry_request)(int request_int) = NULL;
EXPORT_SYMBOL(STUB_ferry_request);

//Syscall to stop ferry
long (*STUB_ferry_stop)(int stop_int) = NULL;
EXPORT_SYMBOL(STUB_ferry_stop);

/* System call wrapper. If the stub is not NULL, it will be run,
otherwise returns -ENOSYS */

asmlinkage long sys_ferry_start(int start_int)
{
	if (STUB_ferry_start)
		return STUB_ferry_start(start_int);
	else
		return -ENOSYS;
}

asmlinkage long sys_ferry_request(int request_int)
{
	if (STUB_ferry_request)
		return STUB_ferry_request(request_int);
	else
		return -ENOSYS;
}

asmlinkage long sys_ferry_stop(int stop_int)
{
	if (STUB_ferry_stop)
		return STUB_ferry_stop(stop_int);
	else
		return -ENOSYS;
}
