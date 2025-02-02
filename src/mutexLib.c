#include<stdio.h>
#include<errno.h>


#include "../include/mutexLib.h"
#include "../include/helper.h"

pthread_mutex_t mutexFileUsers; 
pthread_mutex_t mutexFileMessages; 
pthread_mutex_t mutexBulletinBoard; 


void initMutex(pthread_mutex_t mt)
{
	if(pthread_mutex_init(&mt, NULL)){
		errFunction("Error in mutex init"); 
	}
}

void lockMutex(pthread_mutex_t mt)
{
	
	rd: 
		if(pthread_mutex_lock(&mt)){
			if(errno == EINTR) goto rd; 
			errFunction("Error in mutex lock"); 
		}
}


void unlockMutex(pthread_mutex_t mt)
{
	rd: 
		if(pthread_mutex_unlock(&mt)){
			if(errno == EINTR) goto rd; 
			errFunction("Erro in mutex unlock"); 
		
		}
}
