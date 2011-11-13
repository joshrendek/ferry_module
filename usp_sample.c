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
	ret1=syscall(__NR_ferry_start, test1);
	if(ret1<0)
		perror("system call error");
	else
		printf("Ferry thread started\n");

	//testing request
	/*int test2=35;
	long ret2;
	ret2=syscall(__NR_ferry_request, test2);
	if(ret2<0)
		perror("system call error");
	else
		printf("Function successful, returned %i\n", ret2);

	//testing stop
	int test3=45;
	long ret3;
	ret3=syscall(__NR_ferry_stop, test3);
	if(ret3<0)
		perror("system call error");
	else
		printf("Function successful, returned %i\n", ret3);*/
	return 0;
}
