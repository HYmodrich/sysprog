#include "hybrid_lock.h"
#include <stdlib.h>

int hybrid_lock_init(struct hybrid_lock *lock, const pthread_mutexattr_t *restrict attr)
{
	if(pthread_mutex_init(&lock->h_mutex,attr)!=0){
		printf("lock init error\n");
		exit(1);
	}
	return 0;
}

int hybrid_lock_destroy(struct hybrid_lock* lock)
{
	if(pthread_mutex_destroy(&lock->h_mutex)!=0){
		printf("destroy error\n");
		exit(1);
	}
	return 0;
}

int hybrid_lock_lock(struct hybrid_lock* lock)
{
	//long l_count=0;
	if(pthread_mutex_trylock(&lock->h_mutex)==0)
		return 0;

	struct timeval tv,ttv;
	gettimeofday(&tv,NULL);
	
	int count =0;
	int num=0;
	while(1){
		//l_count++;
		if(pthread_mutex_trylock(&lock->h_mutex)==0){
			//printf("lcount : %ld ",l_count);
			return 0;
			}

		count++;
		if(count >= 1000000 && count%100000 == 0){
			num++;
			gettimeofday(&ttv,NULL);
			if(ttv.tv_usec >= tv.tv_usec && ttv.tv_sec > tv.tv_sec){
				//printf("# of gettimeofday : %d\n",num);
				//printf("%ld // %ld\n",ttv.tv_sec - tv.tv_sec,ttv.tv_usec - 						tv.tv_usec);
				break;
			}
		}
	}
	if(pthread_mutex_lock(&lock->h_mutex)!=0){
		printf("mutex_lock error\n");
		exit(1);
	}
	return 0;

}

int hybrid_lock_unlock(struct hybrid_lock* lock)
{
	if(pthread_mutex_unlock(&lock->h_mutex) !=0){
		printf("unlock fail\n");
		exit(1);
	}
	
	return 0;

}
