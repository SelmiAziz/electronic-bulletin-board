#ifndef FUNCTION_SERVER_H
#define FUNCTION_SERVER_H

#include "commServerClientConfig.h"
#include "messageLib.h"

#define SIZE 1024



//ma perché devo mettere gli spazi ??

typedef struct threadData {
    int socket; 
    BulletinBoard *myBoard; 
} ThreadData; 

void *worker(void *arg); 

#endif // FUNCTION_SERVER_H

