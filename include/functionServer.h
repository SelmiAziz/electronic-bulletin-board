#ifndef FUNCTION_SERVER_H
#define FUNCTION_SERVER_H

#include "commServerClientConfig.h"
#include "messageLib.h"

#define TRY 3
#define TIME_OUT 200
#define MAX_READ_ATTEMPTS 7
#define LISTEN 10
#define SIZE 1024
#define USERS_FILE    "./data/data_users.csv"
#define MESSAGES_FILE "./data/data_messages.csv"
#define SHADOW_FILE "./data/file_shadow.csv"

//ma perché devo mettere gli spazi ??

typedef struct threadData {
    int socket; 
    BulletinBoard *myBoard; 
} ThreadData; 

void *worker(void *arg); 

#endif // FUNCTION_SERVER_H

