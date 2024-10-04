#include <sys/socket.h>       
#include <sys/types.h>        
#include <arpa/inet.h>        
#include <unistd.h>        
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "helper.h"

#define MAX_USERNAME 64
#define MAX_PASSWORD 64

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

void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen){
		memset(buffer + len, '\0', maxLen-len); 
	}

}

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


int main(int argc, char *argv[]) {
    int       connSocket; 
    short int port = 2500; 
    struct    sockaddr_in 
    char     *szAddress;            
    char     *szPort;              
	char *addrIp = "127.0.0.1"; 
	char username[MAX_USERNAME]; 
	char password[MAX_PASSWORD];
	char c;  
	char buffer[1024]; 


    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "client: errore durante la creazione della socket.\n");
		exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);

	//qui ho fatto una connessione usando direttamente l'ip rivedere
 	 if (inet_pton(AF_INET, addrIp, &servaddr.sin_addr) <= 0) {
        perror("Errore nella conversione dell'indirizzo IP");
        exit(EXIT_FAILURE);
    }

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
		printf("client: errore durante la connect.\n");
		exit(EXIT_FAILURE);
     }
    choice:
		printf("Scrivere LOG per il login oppure SUB per la registrazione di un nuovo account\n"); 
		fflush(stdout); 
		if(getInput(buffer, 1024)){
			fprintf(stderr, "Error in reading from stdin\n"); 
			exit(EXIT_FAILURE); 
		}
		if(strcmp(buffer, "LOG") == 0){
			//fa partire il login
		}else if(strcmp(buffe, "SUB") == 0){
			//fa partire la sub seguita dal login
		}

	//presa dell'username e della password; 
	//per gli errori devo creare una funzone apposita
	//questa roba deve essere messa in una funzione
	//questo dentro un while
	rusername: 
		if(getInput(username, MAX_USERNAME)){
			fprintf(stderr,"Error in reading from stdin\n");
			exit(EXIT_FAILURE); 
		}
		
		//c'è bisogno di una funzione write per il client, ma anche per il sever
		write(connSocket, username, MAX_USERNAME); 
		
		readCom(connSocket, &c); 
		
		//di default che faccio in questo switch ? 
		switch(c){
			case COMMAND_ERR_USER_ALREADY_EXISTS : 
				printf("L'utente è già presente nel sistema\n"); 
				break; 
			case COMMAND_SUCCESS: 
				printf("Bene\n"); 
				break;
			case COMMAND_CLOSE: 
				//qua si è chiuso il collegamento
				
				exit(EXIT_SUCCESS); 
				break; 
		}
	
	if(getInput(password, MAX_PASSWORD)){
		fprintf(stderr, "Error in reading from stdin\n"); 
		exit(EXIT_FAILURE); 
	}
	
	
	//questa è proprio merda eh
	switch(c){
		case COMMAND_AUTH : 
			printf("vuole che ti identifichi\n"); 
			writeCom(conn_s, COMMAND_SUB);
			rl: 
			fgets(buffUser, 64, stdin); 
			buffUser[strcspn(buffUser, "\n")] = 0; 
			padBuff(buffUser, strlen(buffUser), 64); 
			write(conn_s, buffUser, 64); 
			printf("Ho inviato bro\n"); 
			fflush(stdout); 
			readCom(conn_s, &c); 
			
			if(c == COMMAND_SUC){
				rl2:
				printf("Inserisci password\n"); 
				fflush(stdout); 
				fgets(buffPassword, 64, stdin); 
				buffPassword[strcspn(buffPassword, "\n")] = 0; 
				padBuff(buffPassword, strlen(buffPassword), 64); 
				write(conn_s, buffPassword, 64); 
				printf("Ho inviato bro la pass"); 
				fflush(stdout); 
				readCom(conn_s, &c); 
				if(c == COMMAND_FAIL){
					printf("Non a buon fine\n"); 
					printf("La pass è priorio %s\n", buffPassword); 
					goto rl2; 
				}else if(c == COMMAND_SUC){
					printf("Andato bene bro\n"); 
				} 
			}else if (c == COMMAND_FAIL){
				printf("Devi ripetere\n"); 
				fflush(stdout); 
				goto rl; 
			}else{
				printf("Che roba è questa\n"); 
				fflush(stdout); 
				while(1); 
			}
			
			//write(conn_s, buffUser, 64); 
			//write(conn_s, password, strlen(password)); 
			break; 
		case COMMAND_SUB: 
			printf("Vuoe guardare dentro la bacheca\n"); 
			fflush(stdout); 
			break; 
		default: 
			printf("Operazione non supportata\n"); 
			fflush(stdout); 
	}
    /*  Output echoed string  */

	while(1); 

    return EXIT_SUCCESS;
}




