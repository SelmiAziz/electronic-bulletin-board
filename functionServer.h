#define "messageLib.h"

#define TRY 3

#define TIME_OUT 5 
#define MAX_READ_ATTEMPTS 7

#define LISTEN 10
#define SIZE 1024

#define COMMAND_AUTH   0x01
#define COMMAND_LOG    0x02
#define COMMAND_SUB    0x03

//sostituire il fail in alcuni casa con resend data 
//i tenativi di quanti farli e meglio averli nel server
#define COMMAND_ERR_USER_NOT_FOUND    0x04
#define COMMAND_ERR_NOT_MATCH_CREDENTIALS     0x05
#define COMMAND_ERR_USER_ALREADY_EXISTS 0x06
#define COMMAND_SUCCESS    0x07
#define COMMAND_CLOSE 0x08


#define COMMAND_POST_MSG 0x09
#define COMMAND_VIEW_MSG 0x10
#define COMMAND_DELETE_MSG 0x11


typedef struct threadData{
	int socket; 
	User *head; 
}ThreadData; 


void *worker(void *arg); 

