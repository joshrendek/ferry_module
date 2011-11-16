#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/unistd.h>

//define syscall numbers
#define __NR_ferry_start 345
#define __NR_ferry_request 346
#define __NR_ferry_stop 347

int main()
{
	//Start ferry
	long ret1 = syscall(__NR_ferry_start);
	if(ret1!=0)
		perror("Ferry_start error");
	else
		printf("Ferry thread started\n");

	//testing request
	long ret2 = syscall(__NR_ferry_request, 2, 'n');
	if(ret2 == 1)
		perror("Issue_Request Error");
	else
		printf("Request successful\n");

	//begin random requests with random wait times

	syscall(__NR_ferry_request, 2, 'n');
	sleep(3);
	syscall(__NR_ferry_request, 0, 'n');
	sleep(2);
	syscall(__NR_ferry_request, 1, 'n');
	syscall(__NR_ferry_request, 1, 'n');
	sleep(1);
	syscall(__NR_ferry_request, 2, 'n');
	syscall(__NR_ferry_request, 2, 'n');
	syscall(__NR_ferry_request, 2, 'n');
	syscall(__NR_ferry_request, 1, 's');
	sleep(4);
	syscall(__NR_ferry_request, 0, 's');
	syscall(__NR_ferry_request, 1, 's');
	syscall(__NR_ferry_request, 2, 's');
	syscall(__NR_ferry_request, 1, 's');
	sleep(1);
	syscall(__NR_ferry_request, 2, 's');
	syscall(__NR_ferry_request, 0, 's');
	sleep(5);
	syscall(__NR_ferry_request, 2, 's');
	syscall(__NR_ferry_request, 1, 's');
	sleep(2);
	syscall(__NR_ferry_request, 1, 's');
	sleep(3);
	syscall(__NR_ferry_request, 0, 'n');
	syscall(__NR_ferry_request, 1, 'n');
	syscall(__NR_ferry_request, 1, 'n');
	sleep(2);
	syscall(__NR_ferry_request, 2, 's');
	syscall(__NR_ferry_request, 0, 'n');
	syscall(__NR_ferry_request, 0, 'n');
	sleep(1);
	syscall(__NR_ferry_request, 2, 's');
	sleep(4);
	syscall(__NR_ferry_request, 0, 'n');
	sleep(6);
	syscall(__NR_ferry_request, 1, 'n');
	sleep(2);
	syscall(__NR_ferry_request, 1, 's');
	sleep(3);
	syscall(__NR_ferry_request, 0, 'n');
	sleep(2);
	syscall(__NR_ferry_request, 2, 's');
	sleep(1);
	syscall(__NR_ferry_request, 2, 's');
	sleep(4);
	syscall(__NR_ferry_request, 0, 'n');
	syscall(__NR_ferry_request, 2, 'n');
	syscall(__NR_ferry_request, 2, 'n');
	syscall(__NR_ferry_request, 2, 'n');
	sleep(6);
	syscall(__NR_ferry_request, 1, 'n');

	//Stopping ferry
	long stop_ret = syscall(__NR_ferry_stop);
	if(stop_ret==1)
		perror("Ferry already stopping");
	else
		printf("Ferry stopped successfully\n");

	return 0;
}
