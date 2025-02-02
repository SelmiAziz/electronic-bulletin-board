#include <sys/socket.h>       
#include <sys/types.h>       
#include <arpa/inet.h>        
#include <unistd.h>           
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/messageLib.h"
#include "../include/helper.h"
#include "../include/fileMessageLib.h"
#include "../include/functionServer.h"
#include "../include/mutexLib.h"
#include "../include/serverConfig.h"



int fd[MAX_NUM_THREADS]; 


int receiveTimeout(int socket)
{
	struct timeval timeout; 
	
	timeout.tv_sec = TIME_OUT; 
	timeout.tv_usec = 0; 
	
	return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

}


void initTimer(struct itimerval timer)
{
    	timer.it_value.tv_sec = CLEAN_TIME;     
    	timer.it_value.tv_usec = 0;
    	timer.it_interval.tv_sec = CLEAN_TIME; 
    	timer.it_interval.tv_usec = 0;


}

void handlerSigint(int signum)
{
	for(int j = 0; j<numThreads; j++){
		close(fd[j]);
	}
	exit(EXIT_SUCCESS); 
}

void periodicFunction(int signum) {
   lockMutex(mutexFileMessages); 
   eliminateZeroPresence(MESSAGES_FILE,SHADOW_FILE); 
   copyFile(SHADOW_FILE,MESSAGES_FILE); 
   unlockMutex(mutexFileMessages); 
}



void fillUsersFromFile(BulletinBoard *myBoard, char *file)
{
	char buffUser[SIZE_USERNAME]; 
	char buffPass[SIZE_PASSWORD]; 
	char buff[SIZE]; 
	
	FILE *myFile = fopen(file, "r"); 
	if(myFile == NULL){
		errFunction("Error opening file"); 
	
	}
	while(fgets(buff, 1024, myFile)){
		fillUser(buff,buffUser, buffPass);
		addUser(myBoard, buffUser, buffPass); 
	} 
	fclose(myFile); 
}

void fillMessagesFromFile(BulletinBoard *myBoard, char *file)
{
	char buffUser[SIZE_USERNAME+1]; 
	char buffObj[SIZE_OBJECT+1]; 
	char buffText[SIZE_TEXT+1]; 
	char buffIdMessage[SIZE_ID_MESSAGE+1]; 
	char buff[SIZE]; 
	int v;
	int numTemp;  
	
	
 	FILE *myFile = fopen(file, "r"); 
 	if(myFile == NULL
 	){
 		errFunction("Error opening file"); 
 	}
 	
	while(fgets(buff, SIZE, myFile))
	{
		fillMsg(buff, buffUser, buffObj, buffText, buffIdMessage, &v);
		if(v!= 0){
			addMessageUser(myBoard, buffUser, buffObj, buffText, buffIdMessage); 
			numTemp = convertStringToNumber(buffIdMessage); 
			if(numTemp > myBoard->idCount) myBoard->idCount = numTemp; 
		}
	}
	fclose(myFile); 
}



int main(int argc, char *argv[]) {  
    struct itimerval timer;  
    struct sigaction sa_int, sa_alarm, sa_ignore;  

    pthread_t pid; 

    int listSocket;  
    int connSocket;  
    struct sockaddr_in serverAddr;  
    struct sockaddr_in clientAddr;  
    int s_size;  
    ThreadData *tData;  
    int port = 2500;  

    char *fileUsers = USERS_FILE;  
    char *fileMessages = MESSAGES_FILE;  
    
  

    sa_int.sa_handler = handlerSigint;  
    sa_int.sa_flags = 0;  
    if( sigaction(SIGINT, &sa_int, NULL) == -1) {  
        errFunction("Error in setting sigaction");  
    }  

    sa_alarm.sa_handler = periodicFunction;  
    sa_alarm.sa_flags = 0;    
    if(sigaction(SIGALRM, &sa_alarm, NULL) == -1) {  
        errFunction("Error in setting sigaction");  
    }  

    sa_ignore.sa_handler = SIG_IGN;  
    if(sigaction(SIGPIPE, &sa_ignore, NULL) == -1) {  
        errFunction("Error in setting sigaction");  
    }  

    // creation of the data structure representing the electronic bulletin board in memory  
    BulletinBoard *myBoard = createBulletinBoard();  

    // loading users and messages from the files  
    fillUsersFromFile(myBoard, fileUsers);  
    fillMessagesFromFile(myBoard, fileMessages);  

    // initializing various mutexes  
    initMutex(mutexFileUsers);  
    initMutex(mutexFileMessages);  
    initMutex(mutexBulletinBoard);  

    // I AM SETTING A TIMER TO CLEAN THE FILE EVERY 5 Seconds Using SIGALRM  
    initTimer(timer);  
    // Start the timer  
    setitimer(ITIMER_REAL, &timer, NULL);  

    if( (listSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
        errFunction("Error in creating the socket");  
    }  
    memset(&serverAddr, 0, sizeof(serverAddr));  
    serverAddr.sin_family = AF_INET;  
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);  
    serverAddr.sin_port = htons(port);  
    
    if(bind(listSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){  
        errFunction("Error in bind");  
    }  
    if(listen(listSocket, LISTEN) < 0){  
        errFunction("Error in listen");  
    }  

    s_size = sizeof(struct sockaddr_in);  

    while(numThreads < MAX_NUM_THREADS){  
        if((connSocket = accept(listSocket, (struct sockaddr *)&clientAddr, &s_size)) < 0 ){  
            errFunction("Error in accept");  
        }  

        if(receiveTimeout(connSocket) < 0){  
            errFunction("Error in timeout");  
        }  
        
        tData = malloc(sizeof(tData));  
        if(tData == NULL){  
            errFunction("Error in malloc");  
        }  


        tData->socket = connSocket;  
        tData->myBoard = myBoard;  

        fd[numThreads] = connSocket; 
        numThreads++;  

        if(pthread_create(&pid, NULL, worker, (void*)tData)){  
            errFunction("Error in creating the thread");  
        }  


    }  

    return 0;  
}

