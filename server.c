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

#include "messageLib.h"
#include "helper.h"
#include "functionServer.h"


int receiveTimeout(int socket){
	struct timeval timeout; 
	timeout.tv_sec = TIME_OUT; 
	timeout.tv_usec = 0; 
	
	
	return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

}


int main(int arcv, char *argv[]){
	int listSocket; 
	int connSocket; 
	struct sockaddr_in serverAddr; 
	struct sockaddr_in clientAddr; 
	pthread_t pid; 
	int s_size; //scoprire che roba è questa
	ThreadData *tData; 
	int port = 2500; 
	char *fileUsers = "back_users.csv"; 
	char *fileMessages = "back_messages.csv"; 
	User *head = NULL; 
	
	//devo trovare il modo di specificare il file che intendo usare
	fillUsers(&head, fileUsers); 
	
	if( (listSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		errFunction("Errore nella creazione della socket"); 
	}

	memset(&serverAddr, 0, sizeof(serverAddr)); 
	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	serverAddr.sin_port = htons(port); 

	if(bind(listSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
		errFunction("Errore nella bind");
	}

	if(listen(listSocket, LISTEN) < 0){
		errFunction("Errore nella listen"); 
	}
	
	s_size = sizeof(struct sockaddr_in); 
	
	while(1){
		if((connSocket = accept(listSocket, (struct sockaddr *)&clientAddr, &s_size))< 0){
			errFuntion("Errore in accpet"); 
		}	
		
		myElement = malloc(sizeof(myElement)); 
		if(myElement == NULL){
			errFunction("Errore in malloc"); 
		}
		
		if(receiveTimeout(connSocket) < 0){
			errFunction("Errore in timeout"); 
		}
		
		tData->socket = connSocket;
		tData->head = head; 
		
		//servirebbe prima fare una sorta di detach modificando gli attributi del thread
		if(pthread_create(&pid, NULL, worker, (void*)myElement)){
			errFunction("Errore nella creazione del thread"); 
		}	
		
		
	} 
	return 0; 
}
