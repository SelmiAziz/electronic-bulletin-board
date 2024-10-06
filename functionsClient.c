#include<unistd.h>

#include<errno.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include"functionsClient.h"
#include"helper.h"


void readCom(int socket, char *buff){
	int ret; 
	ret = read(socket, buff, 1); 
	
	if(ret == 0)
	{
		fprintf(stderr, "connessione chiusa peer\n"); 
		exit(EXIT_FAILURE); 
	}
	if(ret == -1)
	{
		fprintf(stderr,"errore in lettura\n"); 
		exit(EXIT_FAILURE); 
	}
}

void writeCom(int socket, char command){
	int ret; 
	ret = write(socket, &command, 1); 
	if(ret == -1) 
	{
		fprintf(stderr, "errore di scrittura\n"); 
		exit(EXIT_FAILURE); 
	}
}


int readBuffSocket(int socket, char *buff, int len){
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



int writeBuffSocket(int socket, char *buffer, int len)
{
	int l,w; 
	l = len; 
	
	while(l > 0)
	{
		w = write(socket, buffer, len); 
		if( w <= 0)
		{
			if(errno == EINTR) w = 0; 
			else{
				return -1; 
			}
		l -= w; 
		buffer += w; 
		
		}
	}
	return len; 
}

void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen)
	{
		memset(buffer + len, '\0', maxLen-len); 
	}

}

int checkFormatUsername(char *username)
{
	if(strlen(username) > 4) return 0; 
	return -1; 

}

int getValideUsername(char *username)
{
	while(1){
		if(getInput(username, MAX_USERNAME))
		{
			errFunction("Errore di lettura username da stdin"); 
		}
		if(checkFormatUsername(username) == 0) break;
		printf("Lunghezza username deve essere >4"); 
	}
	return 0; 

}


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
int checkFormatPassword(char *password){
	char *specials = "!#$%^&*()-+_="; 
	char *num = "1234567890"; 
	char *alM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 

	if(strlen(password) < 8) return -1; 
	if(controlPass(password, specials) || controlPass(password,num) || controlPass(password, alM)) return -1;
	return 0; 


}

int getValidePassword(char *password){
	while(1){
		if(getInput(password, MAX_PASSWORD))
		{
			errFunction("Errore di lettura password da stdin");
		}
		if(checkFormatPassword(password) == 0) break; 
		printf("Formato password :almeno 8 caratteri tra cui una lettera maiuscola, un numero e un carattere speciale\n"); 
	}
}

int loginFunctionClient(int socket, char *username, char *password){
	char c; 

	while(1)
	{
		printf("Inserisci usarname\n"); 
		getValideUsername(username); 
		
		
		printf("Inserisci password\n"); 
		getValidePassword(password); 
		
		padBuff(username, strlen(username), MAX_USERNAME); 
		writeBuffSocket(socket, username, MAX_USERNAME); 
		padBuff(password, strlen(password), MAX_PASSWORD); 
		writeBuffSocket(socket, password, MAX_PASSWORD); 
		
		readCom(socket, &c); 
		
		switch(c){
			case COMMAND_SUCCESS: 
				printf("Login effettuato con successo\n"); 
				break; 
			case COMMAND_ERR_USER_NOT_FOUND: 
				printf("Utente non presente nel sistema\n");
				break; 
			case COMMAND_ERR_NOT_MATCH_CREDENTIALS: 
				printf("Credenziali non coerenti, inserire le credenziali correttamente\n"); 
				break; 
			default: 
				errFunction("Errore di comunicazione");
		
		}
		if(c == COMMAND_SUCCESS) break; 
	}
	return 0; 
}

int subFunctionClient(int socket, char *username, char *password)
{
	char c; 
	
	while(1)
	{
		printf("Inserisci un username con numero di caratteri tra 8 e 64 (compresi)\n"); 
		getValideUsername(username); 
		
		printf("Inserisci password con numero di caratteri tra 8 e 64 (compresi)\n"); 
		printf("La password deve contenere una un carattere maiuscolo, un numero e un carattere speciale\n"); 
		getValidePassword(password); 
		
		padBuff(username, strlen(username), MAX_USERNAME); 
		writeBuffSocket(socket, username, MAX_USERNAME); 
		padBuff(password, strlen(password), MAX_PASSWORD); 
		writeBuffSocket(socket, password, MAX_PASSWORD); 
		
		
		readCom(socket, &c); 
		
		switch(c){
			case COMMAND_SUCCESS: 
				printf("Registrazione andata a buon fine\n"); 
				break;
			case COMMAND_ERR_USER_ALREADY_EXISTS: 
				printf("L'utente è già registrato nel sistema\n"); 
				break; 
			default: 
				errFunction("Errore di comunicazione"); 
		}
		if( c == COMMAND_SUCCESS) break; 
	}
	return 0;
}

int authFunctionClient(char *username, char *password, int socket){
	char c; 
	char buffer[SIZE]; 
	
	printf("Scrivere SUB per la registrazione oppure LOG per il login\n"); 
	printf("NewVersione");
	while(1){
		if(getInput(buffer, SIZE))
		{
			errFunction("Lettura da stdin non riuscita"); 
		}
		if(strcmp(buffer, "SUB") == 0)
		{
			subFunctionClient(socket, username, password); 
			loginFunctionClient(socket, username, password); 
		}else if(strcmp(buffer, "LOG") == 0)
		{
			//writeBuffSocket(socket,buffer, strlen(buffer)); 
			printf("Inserire SUB (per la registrazione) oppure LOG (per il login)...\n");
		}
	}
	return 0; 
}

int delMessageFunction(int socket)
{




}


int viewMessageFunction(int socket)
{






}


int postMessageFunction(int socket)
{





}






void clientFunc(int socket)
{
	char username[MAX_USERNAME]; 
	char password[MAX_PASSWORD];
	char c;  
	
	readCom(socket, &c); 
	if( c == COMMAND_AUTH ) {
		authFunctionClient(username, password, socket); 
	}else{
		errFunction("Errore di comunicazione"); 
	}
	
	while(1){
		printf("Menu\n"); 
		printf("1:Inserire un nuovo messaggio"); 
		printf("2:Visualizzare tutti i messaggi"); 
		printf("3:Eliminare un messaggio"); 
		
		c = getchar(); 
		while(getchar() != '\n'); 
		
		switch(c){
			case '1': 
				writeCom(socket, COMMAND_POST_MSG); 
				postMessageFunction(socket);
				break; 
			case '2': 
				writeCom(socket, COMMAND_VIEW_MSG); 
				viewMessageFunction(socket);
				break; 
			case '3': 
				writeCom(socket, COMMAND_DELETE_MSG); 
				delMessageFunction(socket); 
				break; 
			default: 
				printf("Opzione del menu non riconosciuta"); 
		}
	}
	
	
}
