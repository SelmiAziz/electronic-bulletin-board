#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#define TRY 3
#define TIME_OUT 200
#define MAX_READ_ATTEMPTS 7
#define LISTEN 10
#define USERS_FILE    "./data/data_users.csv"
#define MESSAGES_FILE "./data/data_messages.csv"
#define SHADOW_FILE "./data/file_shadow.csv"
#define MAX_NUM_THREADS 100
#define CLEAN_TIME 100

extern int numThreads; 


#endif
