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

#include "messageLib.h"
#include "helper.h"
#include "functionServer.h"


int receiveTimeout(int socket){
	struct timeval timeout; 
	timeout.tv_sec = TIME_OUT; 
	timeout.tv_usec = 0; 
	
	
	return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

}


int controlRow(char *buffer)
{
	char *pt = strdup(buffer); 
	char *token = strtok(pt, ","); 
	char *last = NULL; 
	
	while(token)
	{	
		last = token; 
		token = strtok(NULL, ","); 
	
	}

	if(*last ==  '0') return 0; 

	free(pt); 
	pt = NULL; 
	return -1; 

}

void copiaFile(const char* fileSorgente, const char* fileDestinazione) {
    FILE *sorgente, *destinazione;
    char buffer[1024]; // Buffer temporaneo
    size_t bytesRead;

    // Apri i file
    sorgente = fopen(fileSorgente, "r");
    if (sorgente == NULL) {
        perror("Errore nell'aprire il file sorgente");
        exit(EXIT_FAILURE);
    }

    destinazione = fopen(fileDestinazione, "w");
    if (destinazione == NULL) {
        perror("Errore nell'aprire il file destinazione");
        fclose(sorgente);
        exit(EXIT_FAILURE);
    }

    // Copia i contenuti
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), sorgente)) > 0) {
        fwrite(buffer, 1, bytesRead, destinazione);
    }

    // Chiudi i file
    fclose(sorgente);
    fclose(destinazione);

   
}


void eliminateAllZeroRows(char *file)
{
	char buffer[SIZE]; 
	
	FILE *f = fopen(file, "r+");
	FILE *newf = fopen("file_shadow.csv", "w+"); 
	
	rewind(f); 
	while(fgets(buffer, SIZE, f))
	{
		printf("Ho letto %s\n", buffer); 
		if(controlRow(buffer) != 0)
		{
			printf("sto scrivendo akab %s", buffer); 
			fprintf(newf, "%s", buffer); 
		}
	
	}
	fflush(newf);
	fclose(newf); 

}


void funzionePeriodica(int signum) {

   eliminateAllZeroRows("back_messages.csv"); 
   copiaFile("file_shadow.csv", "back_messages.csv"); 
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
	BulletinBoard *myBoard = createBulletinBoard(); 
	
	
	//devo trovare il modo di specificare il file che intendo usare
	fillUsers(myBoard, fileUsers); 
	fillMessagesUsers(myBoard,fileMessages);
	
	
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
