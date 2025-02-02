#ifndef MUTEX_LIB_H
#define MUTEX_LIB_H

#include <pthread.h>

extern pthread_mutex_t mutexFileUsers; 
extern pthread_mutex_t mutexFileMessages; 
extern pthread_mutex_t mutexBulletinBoard; 

void initMutex(pthread_mutex_t mt); 
void lockMutex(pthread_mutex_t mt); 
void unlockMutex(pthread_mutex_t mt); 


#endif
