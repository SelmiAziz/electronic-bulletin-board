#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "messageLib.h"
#include "functionServer.h"


void readCom(int socket, char *buff){
	int ret; 
	ret = read(socket, buff, 1); 
	if(ret == 0){
		fprintf(stderr, "connessione chiusa peer\n"); 
		exit(EXIT_FAILURE); 
	}
	if(ret == -1){
		fprintf(stderr,"errore in lettura\n"); 
		exit(EXIT_FAILURE); 
	}
}

void writeCom(int socket, char command){
	int ret; 
	ret = write(socket, &command, 1); 
	if(ret == -1) {
		fprintf(stderr, "errore di scrittura\n"); 
		exit(EXIT_FAILURE); 
	}
}





//la funzione ritorna -1 errno invariato errore di lettura
//la funzione ritorna 0 canale chiuso
//la funzione ritorna -1 errno uguale a EWOULDBLOCK oppure EAGAIN errore di timeout
int readTimeout(int fd, void *buffer, size_t total_bytes) {
    size_t bytes_read = 0;
    ssize_t result;
    int attempts = 0;

    while (bytes_read < total_bytes) {
        result = read(fd, (char *)buffer + bytes_read, total_bytes - bytes_read);

        if (result < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                attempts++;
                if (attempts == MAX_READ_ATTEMPTS) {
                    return -1;
                }
                continue; 
            }
            return -1; 
        } else if (result == 0) {
            return 0; 
        }

        bytes_read += result;
    }

    return bytes_read; 
}


int subFunctionServer(int socket, char *username, char *password) {
    int ret; 
    while (1) {
        ret = readTimeout(socket, username, password); 
        if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
            writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
            close(socket);
            pthread_exit(NULL); 
        }
        if (ret == -1) {
            writeCom(socket, COMMAND_CLOSE); 
            close(socket);
            pthread_exit(NULL); 
        }

        ret = readBuff(socket, password, MAX_PASSWORD); 
        if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) {
            writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
            close(socket);
            pthread_exit(NULL); 
        }
        if (ret == -1) {
            writeCom(socket, COMMAND_CLOSE); 
            close(socket);
            pthread_exit(NULL); 
        }

        if (findUser(head, username)) {
            writeCom(socket, COMMAND_ERR_USER_ALREADY_EXISTS);
        } else {
            writeCom(socket, COMMAND_SUCCESS);
            break;
        }
    }

    addUser(&head, username, password); 
    wrUser(username, password, "back_users.csv"); 
    return 0; 
}


void logFunctionServer(int socket, char *username, char *password){
	
	int ret;  
	while(1){
		ret = readBuff(socket, username, MAX_USERNAME); 
		if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
		{
    		writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
        close(socket);
        pthread_exit(NULL); 
    }
    if (ret == -1) 
    {
			writeCom(socket, COMMAND_CLOSE); 
      close(socket);
      pthread_exit(NULL); 
    }
		ret = readBuff(socket, password, MAX_PASSWORD); 
		if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
		{
    		writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
        close(socket);
        pthread_exit(NULL); 
    }
    if (ret == -1) 
    {
			writeCom(socket, COMMAND_CLOSE); 
      close(socket);
      pthread_exit(NULL); 
    }
		
		if(!findUser(head,username)){
			writeCom(socket,COMMAND_ERR_USER_NOT_FOUND); 
			continue; 
		}else{
			writeCom(socket,COMMAND_SUCCESS); 
			break; 
		}
		if(checkUserPass(username, password)){
			writeCom(socket, COMMAND_ERR_NOT_MATCH_CREDENTIALS); 
		}else{
			break; 
		}
	}
}




void authFuncServer(int socket, char *username, char *password){	
	char c; 
	//meglio così che è brutto che il client inizi a manda roba a valanga
	//ci sarebbe da cambiare tutte le funzioni devono essere delle int
	writeCom(socket, COMMAND_AUTH);
	int ret ;  
	readCom(socket, &c); 
	//non dovrebbe mandare qulcosa per dire al client che è andato a buon fine il tutto ?
	if( c == COMMAND_SUB){
			subFuntionServer(socket, username, password); 
			logFunctionServer(socket, username, password); 
	}else if( c == COMMAND_LOG){
			logFunctionServer(socket, username, password); 
	}else{
		//mi hai mandato roba che non era vera
		writeCom(socket, COMMAND_CLOSE); 
	}
}

void *worker(void *arg){
	Element *tData = (ThreadData*) arg; 
	char username[MAX_USERNAME]; 
	char password[MAX_PASSWORD]; 
	char c; 
	int socket = tData->socket; 
	Head *head = tData->head;
	
	free(tData); 
	tData == NULL; 
	authFuncServer(socket, username, password);
	  //ulima interezione la da il server
	readCom(socket, &c); 
	
	//core 
	
	while(1){
		readCom(socket, &c);
		switch(c){
			//case COMMAND_VIEW_MSG: 
			//funzione che manda tutti gli oggetti
			//dopo manda un COMMAND_SUCCESS oppure prima mando il numero (come lo esprimo il numero scusa oppure mando mando fin quando alla fine mando success per dire che ho finito)
			//ma se gli manda le stringhe che rappresentano i messaggi e i testi come fa a tenerli dentro il sistema il client, costruisce una struttura oppure printa tutto al volo?
			//meglio mischiarli entrambi
			//riceve una stringa che è fatta padding per indicare il messaggio in questione
		
			//posso chiedere più volte di vedere un messaggio	
			break; 
			//case COMMAND_POST_MSG: 
				//attendo n byte per il oggetto del messaggio
				//attendo m byte per il testo messaggio
				//mand un success
				
			break; 
			//case COMMAND_DELETE_MSG: 
				//ricevo una stringa che indica id-messaggio
				//lo elmino subito oppure aspetto per eliminarlo ?
				//faccio cancellazione in base all'username
				//se non lo puoi cancellare allora mandi una flag per dire che tutto è andato male
				//elminato mando un success
			break; 
			default: 
				writeCom(socket, COMMAND_FAIL); 
		
		}	
	}
}
