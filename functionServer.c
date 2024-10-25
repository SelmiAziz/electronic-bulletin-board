#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

#include<pthread.h>
#include <unistd.h>   

#include"functionServer.h"
#include"helper.h"



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

void unpadding(char *buff, int len){
	for(int i = strlen(buff)-1; i>0; i--){
		if(buff[i] != ' ') break; 
		buff[i] = '\0'; 
	}

}


void extractAuthMessage(char *s, char *username, char *password) {
    int i, l = 0; 
    for(i = 0; i < SIZE_USERNAME && s[i] != ' '; i++) {
        username[i] = s[i]; 
    }
    username[i] = '\0'; 
    
  	for(i = SIZE_USERNAME +1; s[i] != ' ' && s[i] != '\0' ; i++){
 		password[l] = s[i]; 
 		l++; 
  	}
  	password[l] = '\0'; 
}

int subFunctionServer(int socket, User *head, char *username, char *password) {
    int ret; 
    char authMessage[SIZE_AUTH_MESSAGE]; 
    
    while (1) {
       ret = readTimeout(socket, authMessage, SIZE_AUTH_MESSAGE); 
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
        
    	extractAuthMessage(authMessage, username, password); 
   
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


int logFunctionServer(int socket, User *head, char *username, char *password){
	char authMessage[SIZE_AUTH_MESSAGE]; 
	int ret;  
	
	while(1){
		ret = readTimeout(socket, authMessage, SIZE_AUTH_MESSAGE); 
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

	
		extractAuthMessage(authMessage, username, password); 
		
		if(findUser(head,username) == NULL){
			writeCom(socket,COMMAND_ERR_USER_NOT_FOUND); 
			continue; 
		}
		if(checkUserPass(head, username, password)){
			writeCom(socket, COMMAND_ERR_NOT_MATCH_CREDENTIALS); 
		}else{
			writeCom(socket,COMMAND_SUCCESS); 
			break; 
		}
	}
	return 0; 
}




int authFuncServer(int socket, User *head, char *username, char *password){	
	char c; 
	//meglio così che è brutto che il client inizi a manda roba a valanga
	//ci sarebbe da cambiare tutte le funzioni devono essere delle int
	writeCom(socket, COMMAND_AUTH);
	printf("HO mandato un FRA\n"); 
	fflush(stdout); 
	int ret ;  
	readCom(socket, &c); 
	//non dovrebbe mandare qulcosa per dire al client che è andato a buon fine il tutto ?
	if( c == COMMAND_SUB){
			subFunctionServer(socket, head, username, password); 
			logFunctionServer(socket, head, username, password); 
	}else if( c == COMMAND_LOG){
			logFunctionServer(socket, head, username, password); 
	}else{
		//mi hai mandato roba che non era vera
		writeCom(socket, COMMAND_CLOSE); 
	}
	return 0; 
}

void *worker(void *arg){
	ThreadData *tData = (ThreadData*) arg; 
	char username[SIZE_USERNAME+1]; 
	char password[SIZE_PASSWORD+1]; 
	char c; 
	int socket = tData->socket; 
	User *head = tData->head;
	
	free(tData); 
	tData == NULL; 
	authFuncServer(socket, head, username, password);
	
	while(1){
		readCom(socket, &c);
		switch(c){
			case COMMAND_QUIT: 
				pthread_exit(NULL); 
			break; 
		
			case COMMAND_VIEW_MSG: 
			//funzione che manda tutti gli oggetti
			//dopo manda un COMMAND_SUCCESS oppure prima mando il numero (come lo esprimo il numero scusa oppure mando mando fin quando alla fine mando success per dire che ho finito)
			//ma se gli manda le stringhe che rappresentano i messaggi e i testi come fa a tenerli dentro il sistema il client, costruisce una struttura oppure printa tutto al volo?
			//meglio mischiarli entrambi
			//riceve una stringa che è fatta padding per indicare il messaggio in questione
		
			//posso chiedere più volte di vedere un messaggio	
			break; 
			case COMMAND_POST_MSG: 
				//attendo n byte per il oggetto del messaggio
				//attendo m byte per il testo messaggio
				//mand un success
				
			break; 
			case COMMAND_DELETE_MSG: 
				//ricevo una stringa che indica id-messaggio
				//lo elmino subito oppure aspetto per eliminarlo ?
				//faccio cancellazione in base all'username
				//se non lo puoi cancellare allora mandi una flag per dire che tutto è andato male
				//elminato mando un success
			break; 
			default: 
				writeCom(socket, COMMAND_CLOSE); 
		
		}	
	}
}
