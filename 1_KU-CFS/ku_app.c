#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
	if(argc != 2){
		printf("ku_app: Wrong number of arguments\n");
		exit(1);
	}
	if(strlen(argv[1]) != 1){
		printf("ku_app: Invalid argument value\n");
		exit(1);
	}
	if(argv[1][0] < 'A' || argv[1][0] > 'Z'){
		printf("ku_app: Invalid argument value\n");
		exit(1);
	}

	kill(getpid(), SIGSTOP);

	while(1){
		fprintf(stderr, "%c\n", argv[1][0]);
		usleep(200000);
	}

	return 0;
}
