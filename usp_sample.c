#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <linux/unistd.h>

#define __NR_ferry_start 345
#define __NR_ferry_request 346
#define __NR_ferry_stop 347

int main()
{
	//testing start
	int test1=25;
	long ret1;
	ret1=syscall(__NR_ferry_start);
	if(ret1!=0)
		perror("Ferry_start error");
	else
		printf("Ferry thread started\n");

	//testing request
	int request1=2;
	char request2='n';
	long ret2;
	ret2=syscall(__NR_ferry_request, request1, request2);
	if(ret2==1)
		perror("Issue_Request Error");
	else
		printf("Request successful\n");

	long ret3 = syscall(__NR_ferry_request, 2, 'n');
	long ret4 = syscall(__NR_ferry_request, 0, 'n');
	long ret5 = syscall(__NR_ferry_request, 1, 'n');
	long ret6 = syscall(__NR_ferry_request, 1, 'n');
	long ret7 = syscall(__NR_ferry_request, 2, 'n');
	long ret8 = syscall(__NR_ferry_request, 2, 'n');
	long ret9 = syscall(__NR_ferry_request, 2, 'n');
	long ret10 = syscall(__NR_ferry_request, 1, 's');
	long ret11 = syscall(__NR_ferry_request, 0, 's');
	long ret12 = syscall(__NR_ferry_request, 1, 's');
	long ret13 = syscall(__NR_ferry_request, 2, 's');
	long ret14 = syscall(__NR_ferry_request, 1, 's');
	long ret15 = syscall(__NR_ferry_request, 2, 's');
	long ret16 = syscall(__NR_ferry_request, 0, 's');
	long ret17 = syscall(__NR_ferry_request, 2, 's');
	long ret18 = syscall(__NR_ferry_request, 1, 's');

	sleep(25);

	long stop_ret = syscall(__NR_ferry_stop);
	if(stop_ret==1)
		perror("Ferry already stopping");
	else
		printf("Ferry stopped successfully\n");
	//testing stop
	/*int test3=45;
	long ret3;
	ret3=syscall(__NR_ferry_stop, test3);
	if(ret3<0)
		perror("system call error");
	else
		printf("Function successful, returned %i\n", ret3);*/
	return 0;
}
