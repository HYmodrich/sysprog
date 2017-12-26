#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
	int i;
	int count = 0;
	struct timeval start_time, end_time;

	gettimeofday(&start_time,NULL);
	
	while(count!=480000000)
		count++;
	gettimeofday(&end_time,NULL);
	printf("time : %lu sec\n", (end_time.tv_sec - start_time.tv_sec)*1000000  			+ end_time.tv_usec - start_time.tv_usec); 
	return 0;
}
