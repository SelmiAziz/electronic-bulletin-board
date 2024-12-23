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

int receiveTimeout(int socket){
	struct timeval timeout; 
	timeout.tv_sec = TIME_OUT; 
	timeout.tv_usec = 0; 
	
	
	return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

}


void funzionePeriodica(int signum) {

   eliminateZeroPresence("data/back_messages.csv","data/file_shadow.csv"); 
   copyFile("data/file_shadow.csv", "data/back_messages.csv"); 
}



//PARTE INERENTE AI FILE
void fillUsersFromFile(BulletinBoard *myBoard, char *file)
{
	char buffUser[SIZE_USERNAME]; 
	char buffPass[SIZE_PASSWORD]; 
	char buff[SIZE]; 
	
	FILE *myFile = fopen(file, "r"); 
	if(myFile == NULL){
		fprintf(stderr, "Error opening file!"); 
		exit(EXIT_FAILURE); 
	
	}
	while(fgets(buff, 1024, myFile)){
		fillUser(buff,buffUser, buffPass);
		printf("Sto prendendo %s %s\n", buffUser, buffPass); 
		fflush(stdout);
		addUser(myBoard, buffUser, buffPass); 
	} 
	fclose(myFile); 
}

//STO PER USARE UNA FUNZIONE CHE IN REALTÀ USA ANCHE FILEMESSAGELIB.C RIMETTERE TUTTO APPOSTO 



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
 		fprintf(stderr, "Error opening file"); 
 		exit(EXIT_FAILURE); 
 	}
 	
	while(fgets(buff, SIZE, myFile))
	{
		fillMsg(buff, buffUser, buffObj, buffText, buffIdMessage, &v);
		if(v!= 0){
			//il nome non mi piace
			addMessageUser(myBoard, buffUser, buffObj, buffText, buffIdMessage); 
			numTemp = convertStringToNumber(buffIdMessage); 
			if(numTemp > myBoard->idCount) myBoard->idCount = numTemp; 
		}
	}
	fclose(myFile); 



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
	//si dovrebbe specificare tutto in un file
	char *fileUsers = "data/back_users.csv"; 
	char *fileMessages = "data/back_messages.csv"; 
	BulletinBoard *myBoard = createBulletinBoard(); 
	
	
	//devo trovare il modo di specificare il file che intendo usare
	//le modfiche che sto apportando sono in questa parte
	fillUsersFromFile(myBoard,fileUsers); 
	fillMessagesFromFile(myBoard,fileMessages);
	
	
	//STO FISSANDO UN TIMERR PER FAR PULIRE IL FILE OGNI 5 Secondi Usando SIGALARM
	
	struct itimerval timer;

    // Configura il timer: intervallo di 5 secondi
    //DA METTERE DELLE FLAG
    timer.it_value.tv_sec = 5;     // Primo trigger dopo 5 secondi
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 5; // Ripetizione ogni 5 secondi
    timer.it_interval.tv_usec = 0;

    // Associa il segnale SIGALRM alla funzionePeriodica
    signal(SIGALRM, funzionePeriodica);

    // Avvia il timer
    setitimer(ITIMER_REAL, &timer, NULL);
	

	
	
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
			errFunction("Errore in accpet"); 
		}	
		
		tData = malloc(sizeof(tData)); 
		if(tData == NULL){
			errFunction("Errore in malloc"); 
		}
		
		if(receiveTimeout(connSocket) < 0){
			errFunction("Errore in timeout"); 
		}
		
		tData->socket = connSocket;
		tData->myBoard = myBoard;
		
		//servirebbe prima fare una sorta di detach modificando gli attributi del thread
		if(pthread_create(&pid, NULL, worker, (void*)tData)){
			errFunction("Errore nella creazione del thread"); 
		}	
		
	} 
	return 0; 
}
