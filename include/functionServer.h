#ifndef FUNCTION_SERVER_H
#define FUNCTION_SERVER_H

#include "commServerClientConfig.h"
#include "messageLib.h"

#define TRY 3
#define TIME_OUT 200
#define MAX_READ_ATTEMPTS 7
#define LISTEN 10
#define SIZE 1024

//ma perché devo mettere gli spazi ??

typedef struct threadData {
    int socket; 
    BulletinBoard *myBoard; 
} ThreadData; 

void *worker(void *arg); 

#endif // FUNCTION_SERVER_H

