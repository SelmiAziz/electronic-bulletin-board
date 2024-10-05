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
#include "functionsClient.h"



int main(int argc, char *argv[]) {
    int       connSocket; 
    struct    sockaddr_in serverAddress; 
    char     *serverIp;                        
	short int port; 


	parserLine(argc, argv, &serverIp, &port); 
	
	printf("L'indirizzo ip è %s la porta invece %d\n", serverIp, port); 

    if ( (connSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "client: errore durante la creazione della socket.\n");
		exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_port        = htons(port);


	//questa funzione converte stringa indirizzo ip in formato binario
 	 if (inet_pton(AF_INET, serverIp, &serverAddress.sin_addr) <= 0) {
        perror("Errore nella conversione dell'indirizzo IP");
        exit(EXIT_FAILURE);
    }

    if ( connect(connSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress) ) < 0 ) {
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
		}else if(strcmp(buffer, "SUB") == 0){
			//fa partire la sub seguita dal login
		}

	//presa dell'username e della password; 
	//per gli errori devo creare una funzone apposita
	//questa roba deve essere messa in una funzione
	//questo dentro un while
	
	
	
	rusername: 
		if(getInput(username, MAX_USERNAME)){
			errFunction("Error di lettura da stdin", EXIT_FAILURE); 
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
			writeCom(connSocket, COMMAND_SUB);
			rl: 
			fgets(buffer, 64, stdin); 
			//buffUser[strcspn(buffUser, "\n")] = 0; 
			//padBuff(buffUser, strlen(buffUser), 64); 
			//write(conn_s, buffUser, 64); 
			printf("Ho inviato bro\n"); 
			fflush(stdout); 
			//readCom(conn_s, &c); 
			
			if(c == COMMAND_SUCCESS){
				rl2:
				printf("Inserisci password\n"); 
				fflush(stdout); 
				//fgets(buffPassword, 64, stdin); 
				//buffPassword[strcspn(buffPassword, "\n")] = 0; 
				//padBuff(buffPassword, strlen(buffPassword), 64); 
				//write(conn_s, buffPassword, 64); 
				printf("Ho inviato bro la pass"); 
				fflush(stdout); 
				//readCom(conn_s, &c); 
				if(c == COMMAND_CLOSE){
					printf("Non a buon fine\n"); 
					//printf("La pass è priorio %s\n", buffPassword); 
					goto rl2; 
				}else if(c == COMMAND_SUCCESS){
					printf("Andato bene bro\n"); 
				} 
			}else if (c == COMMAND_CLOSE){
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




