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

#define TRY 3

#define TIME_OUT 5 
#define MAX_READ_ATTEMPS 7

#define LISTEN 10

#define COMMAND_AUTH   0x01
#define COMMAND_LOG    0x02
#define COMMAND_SUB    0x03

//sostituire il fail in alcuni casa con resend data 
//i tenativi di quanti farli e meglio averli nel server
#define COMMAND_ERR_USER_NOT_FOUND    0x04
#define COMMAND_ERR_NOT_MATCH_CREDENTIALS     0x05
#define COMMAND_ERR_USERS_ALREADY_EXISTS 0x06
#define COMMAND_SUCCESS    0x07
#define COMMAND_FAIL 0x08

//strutture dati questa parte

User *head = NULL; 



//server una flag per il log out

typedef struct element{
	int socket; 
}Element; 

char *file = "testo.txt"; 

//queste funzioni devono tiornare un intero
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

//i don't like it
int checkUserPass(char *user, char *pass){
	FILE *f; 
	char password[MAX_PASSWORD]; 
	char username[MAX_USERNAME]; 
	f = fopen(file,"r"); 
	if(f == NULL){
		fprintf(stderr, "Error in opening file\n"); 
		exit(EXIT_FAILURE); 
	}
	rewind(f); 
	//usare un fgets con token
	while(fscanf(f, "%s , %s", username, password) != EOF){
		if(strcmp(user, username)== 0){
			if(strcmp(pass, password) == 0){
				return 0; 
			}
	}
	return -1; 
}

//funzioni di lettura di altri campi che non siano i comandi singoli byte

int readBuff(int socket, char *buff, int len){
    int ret, i;  
    char c; 
    for( i = 1; i<len; i++){
    	ret = read(socket, &c, 1); 
    	if(ret == 1){
    		*buff++= c; 
    		if( c == '\n') break; 
    	}else{
    		if(ret == 0){
    			if(i== 1) return 0; 
    			else break; 
    		}else{
    			if(errno == EINTR) continue; 
    			return -1; 
    		}	
    	}	
    }
   *buff = 0; 
   return i; 
}

//this snip has to be in the client
int controlPass(char *password, char *seq){
	for(int i = 0; i<strlen(password); i++){
		for(int j = 0; j<strlen(seq); j++){
			if(password[i] == seq[j]){
				return 0; 
			}
		}
	}
	return -1; 

}

//this password has to implemented in the client is stupid to implement it in the user.
int validatePassword(char *password){
	if(strlen(password) < 8) return -1; 
	char *specials = "!#$%^&*()-+_="; 
	char *num = "1234567890"; 
	char *alM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	if(controlPass(password, specials) || controlPass(password,num) || controlPass(password, alM)) return -1;
	return 0; 
}



int receiveTimeout(int socket){
	struct timeval timeout; 
	timeout.v_sec = TIME_OUT; 
	timeout.tv_usec = 0; 
	
	
	return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

}

int read_with_timeout(int fd, void *buffer, size_t total_bytes) {
    size_t bytes_read = 0;
    ssize_t result;
    int attempts = 0;

    while (bytes_read < total_bytes && attempts < MAX_READ_ATTEMPTS) {
        result = read(fd, (char *)buffer + bytes_read, total_bytes - bytes_read);

        if (result < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                printf("Timeout durante la lettura. Tentativo %d di %d\n", attempts + 1, MAX_READ_ATTEMPTS);
                attempts++;
                continue;  // Riprova a leggere
            } else {
                perror("Errore durante la lettura"); //qua si potrbbe fa chiudere il server
                return -1;  // Errore
            }
        } else if (result == 0) {
            // Connessione chiusa
            printf("Connessione chiusa dal peer\n");
            return -1;  // Errore
        }

        // Aggiungi i byte letti al totale
        bytes_read += result;
        printf("Bytes letti finora: %zu\n", bytes_read);
    }

    // Se non sono stati letti tutti i byte richiesti
    if (bytes_read < total_bytes) {
        printf("Errore: non sono stati letti tutti i byte richiesti. Lettura incompleta: %zu/%zu byte\n", bytes_read, total_bytes);
        // Se abbiamo raggiunto il numero massimo di tentativi, gestiamo l'errore
        if (attempts >= MAX_READ_ATTEMPTS) {
            printf("Numero massimo di tentativi di lettura raggiunto: %d\n", MAX_READ_ATTEMPTS);
            return -1;  // Errore
        }
    }

    return bytes_read == total_bytes ? 0 : -1;  // Restituisce 0 per successo, -1 per errore
}



void sub_func(int socket, char *username, char *password){
	int ret; 
	rusername:
		ret = read(socket,username, MAX_USERNAME); 
		if(ret != MAX_USERNAME){
			//ho ricevuto meno byte, che faccio? per adesso mi tengo il fail
			writeCom(socket, COMMAND_FAIL); 
			goto ruser; 
		}
		if(findUser(head,username)){
			writeCom(socket, COMMAND_ERR_USER_ALREADY_EXISTS); 
			goto ruser; 
		}else{
			writeCom(socket, COMMAND_SUCCESS); 
		}
	rpassword:
		ret = readBuff(socket, password, MAX_PASSWORD); 
		if(ret != MAX_PASSWORD){
			writeCom(socket, COMMAND_FAIL); 
			goto rpassword; 
		} 
		addUser(&head,username, password); 
		wrUser(username, password, "back_users.csv"); 
}


void log_func(int socket, char *username, char *password){
	int ret;  
	rusername:
		ret = readBuff(socket, username, MAX_USERNAME); 
		if(ret != MAX_USERNAME){
			writeCom(socket, COMMAND_FAIL); 
			goto rusername; 
		}
		if(!findUser(head,username)){
			writeCom(socket,COMMAND_ERR_USER_NOT_FOUND); 
			goto rusername;
		}else{
			writeCom(socket,COMMAND_SUCCESS); 
		}
	rpassword:
		ret = readBuff(socket, password, MAX_PASSWORD); 
		if(ret != MAX_PASSWORD){
			writeCom(socket, COMMAND_FAIL); 
			goto rpassword; 
		}
		if(checkUserPass(username, password)){
			writeCom(socket, COMMAND_ERR_NOT_MATCH_CREDENTIALS); 
			goto rusername; 
		}
}

void authFunc(int socket, char *username, char *password){	
	char c; 
	//ma perché mando il segnale dal server e se lo mandasse il client ?
	writeCom(socket, COMMAND_AUTH);
	int ret ;  
	readCom(socket, &c); 
	if( c == COMMAND_SUB){
			sub_func(socket, username, password); 
			log_func(socket, username, password); 
	}else if( c == COMMAND_LOG){
			log_func(socket, username, password); 
	}else{
		writeCom(socket, COMMAND_FAIL); 
	}

}

//mancano le funzioni di unpadding
//il mettere in memoria di lavoro le strutture 
//il fare ricerca sulle strutture
//il inserire o togliere dalle strutture

void *worker(void *arg){
	Element *myElement = (Element*) arg; 
	int socket = myElement->socket; 
	char username[MAX_USERNAME]; 
	char password[MAX_PASSWORD]; 
	char c; 
	
	free(myElement); 
	myElement == NULL; 
	authFunc(socket, username, password);
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

int main(int arcv, char *argv[]){
	int listSocket; 
	int connSocket; 
	struct sockaddr_in serverAddr; 
	struct sockaddr_in clientAddr; 
	pthread_t pid; 
	int s_size; //scoprire che roba è questa
	Element *myElement; 
	int port = 2500; 
	char fileUser = "back_users.csv"; 
	
	//devo trovare il modo di specificare il file che intendo usare
	fillUsers(&head, fileUsers); 
	
	if( (listSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Socket error!"); 
		exit(-1); 
	}
	 

	memset(&server_addr, 0, sizeof(server_addr)); 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	server_addr.sin_port = htons(port); 

	if(bind(listSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
		fprintf(stderr, "Bind error"); 
		exit(-1); 
	}

	if(listen(listSocket, LISTEN) < 0){
		fprintf(stderr, "Listen error"); 
		exit(-1); 
	}
	
	s_size = sizeof(struct sockaddr_in); 
	
	while(1){
		if((connSocket = accept(listSocket, (struct sockaddr *)&clientAddr, &s_size))< 0){
			fprintf(stderr, "Error in accept\n"); 
			exit(EXIT_FAILURE); 
		}	
		
		myElement = malloc(sizeof(myElement)); 
		if(myElement == NULL){
			fprintf(stderr, "Malloc error\n"); 
			exit(EXIT_FAILURE); 
		}
		
		if(receiveTimeout(connSocket) < 0){
			fprintf(stderr, "Errore timeout\n"); 
			exit(EXIT_FAILURE); 
		}
		
		myElement->socket = connSocket;
		
		
		//servirebbe prima fare una sorta di detach modificando gli attributi del thread
		if(pthread_create(&pid, NULL, worker, (void*)myElement)){
			fprintf(stderr, "Error in thread creation"); 
			exit(EXIT_FAILURE);
		}	
		
		
	} 
	return 0; 
}
