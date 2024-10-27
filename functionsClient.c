#include<unistd.h>

#include<errno.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include"functionsClient.h"
#include"helper.h"


int readCom(int socket, char *command){
	return read(socket, command, 1); 
}

int writeCom(int socket, char command){
	return write(socket, &command, 1); 
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


//questa funzione mi ha provocato problemi
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

static int checkFormatUsername(char *username)
{
	if(strlen(username) > 4) return 0; 
	return -1; 

}

static void getValideUsername(char *username)
{
	while(1){
		if(getInput(username, SIZE_USERNAME+1))
		{
			errFunction("Errore di lettura username da stdin"); 
		}
		if(checkFormatUsername(username) == 0) break;
		printf("Lunghezza username deve essere >4"); 
	}

}


static int controlPass(char *password, char *seq){
	for(int i = 0; i<strlen(password); i++){
		for(int j = 0; j<strlen(seq); j++){
			if(password[i] == seq[j]){
				return 0; 
			}
		}
	}
	return -1; 

}

static int checkFormatPassword(char *password){
	char *specials = "!?#$%^&*()-+_="; 
	char *num = "1234567890"; 
	char *alM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	if(strlen(password) < 8) return -1; 
	if(controlPass(password, specials) || controlPass(password,num) || controlPass(password, alM)) return -1;
	return 0; 


}

static void getValidePassword(char *password){
	while(1){
		if(getInput(password, SIZE_PASSWORD+1))
		{
			errFunction("Errore di lettura password da stdin");
		}
		if(checkFormatPassword(password) == 0) break; 
		printf("Formato password :almeno 8 caratteri tra cui una lettera maiuscola, un numero e un carattere speciale\n"); 
	}
}

//anche questa funzione sembra da helper.c
void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen)
	{
		memset(buffer + len, ' ', maxLen-len); 
	}
	buffer[maxLen-1] = '\0'; 
}

void buildMessage(char *bMessage, char *s1, char *s2, int len)
{
	sprintf(bMessage, "%s %s", s1, s2); 
	bMessage[len] = 0; 
}




static void loginFunctionClient(int socket, char *username, char *password){

	int ret; 
	char c; 
	char authMessage[SIZE_AUTH_MESSAGE]; 

	while(1)
	{
		printf("Inserisci usarname\n"); 
		getValideUsername(username); 
		
		
		printf("Inserisci password\n"); 
		getValidePassword(password); 
		
		padBuff(username, strlen(username), SIZE_USERNAME+1); 
		padBuff(password, strlen(password), SIZE_PASSWORD+1); 
		
		buildMessage(authMessage, username, password,SIZE_AUTH_MESSAGE); 
   		
   		//sviluppare una write che funzioni
   		write(socket, authMessage, sizeof(authMessage)); 
	 
		ret = readCom(socket, &c); 
		
		if(ret == 0){
			errFunction("Connessione chiusa dal peer"); 
		}else if(ret == -1){
			errFunction("Errore di lettura dalla socket"); 
		}
		
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
				errFunction("Errore di comunicazione3");
		
		}
		if(c == COMMAND_SUCCESS) break; 
	}
}

static void subFunctionClient(int socket, char *username, char *password)
{
	
	int ret; 
	char c; 
	char authMessage[SIZE_AUTH_MESSAGE]; 
	
	while(1)
	{
		printf("Inserisci un username con numero di caratteri tra 8 e 64 (compresi)\n"); 
		getValideUsername(username); 
		
		printf("Inserisci password con numero di caratteri tra 8 e 64 (compresi)\n"); 
		printf("La password deve contenere una un carattere maiuscolo, un numero e un carattere speciale\n"); 
		getValidePassword(password); 
		
 
		padBuff(username, strlen(username), SIZE_USERNAME+1);
		padBuff(password, strlen(password), SIZE_PASSWORD+1);
   
   		buildMessage(authMessage, username, password,SIZE_AUTH_MESSAGE); 
   		
   		//sviluppare una write che funzioni
   		write(socket, authMessage, sizeof(authMessage)); 
   		
   		ret = readCom(socket, &c);
   		
		if(ret == 0){
			errFunction("Connessione chiusa dal server"); 
		}else if(ret == -1){
			errFunction("Errore di lettura dalla socket"); 
		}		
		
		switch(c){
			case COMMAND_SUCCESS: 
				printf("Registrazione andata a buon fine\n"); 
				break;
			case COMMAND_ERR_USER_ALREADY_EXISTS: 
				printf("L'utente è già registrato nel sistema\n"); 
				break; 
			default: 
				errFunction("Errore di comunicazione2"); 
		}
		if( c == COMMAND_SUCCESS) break; 
	}

}

static void authFunctionClient(char *username, char *password, int socket){

	char c; 
	char buffer[SIZE]; 
	
	printf("Scrivere SUB per la registrazione oppure LOG per il login\n"); 
	
	while(1){
		if(getInput(buffer, SIZE+1))
		{
			errFunction("Lettura da stdin non riuscita"); 
		}
		if(strcmp(buffer, "SUB") == 0)
		{
			if(writeCom(socket, COMMAND_SUB) == -1)
			{
				errFunction("Errore di scrittura sulla socket"); 
			}
			
			subFunctionClient(socket, username, password); 
			loginFunctionClient(socket, username, password); 
			break; 
			
		}else if(strcmp(buffer, "LOG") == 0)
		{
			if(writeCom(socket, COMMAND_LOG) == -1)
			{
				errFunction("Errore di scrittura sulla socket"); 
			}
			
			loginFunctionClient(socket, username, password); 
			break; 
			
		}else{
			printf("Inserire SUB (per la registrazione) oppure LOG (per il login)...\n");
		}
	}
}

static void delMessageFunction(int socket)
{




}


static void printMessage(char *message)
{

	char objBuffer[SIZE_OBJECT+1]; 
	char textBuffer[SIZE_TEXT+1]; 
	char authorBuffer[SIZE_USERNAME+1]; 
	//sarebbe meglio mandare anche un codice con il messaggio in modo da indentificarlo
	
	
	
	//should slplit the message
	
	printf("<-----Messaggio----->"); 
	printf("\n"); 
	printf("Oggetto:%s",objBuffer); 
	printf("\n"); 
	printf("Testo:%s",textBuffer); 
	printf("\n"); 
	printf("Autore:%s",authorBuffer);; 
	printf("\n"); 
	printf("<------------------->"); 



}


static void viewMessageFunction(int socket)
{

	int ret; 
	char c; 
	char numMsgBuff[SIZE_NUM_MSG+1];
	char msgMessage[SIZE_MSG_COMPLETE_MESSAGE];
	 
	int n; 
	//serve una read per 


	ret = readBuffSocket(socket, numMsgBuff, SIZE_NUM_MSG) ; 
	
	if(ret == 0)
	{
		errFunction("Connessione chiusa dal server"); 	
	}else if( ret == -1)
	{
		errFunction("Errore di lettura dalla socket"); 
	}
	
	numMsgBuff[SIZE_NUM_MSG] = 0; 
	
	//converti la strina in numero
	
	n = strtol(numMsgBuff, NULL, 10); 

	for(int i = 0; i<n; i++){
		if(readBuffSocket(socket, msgMessage, SIZE_MSG_MESSAGE) == -1){
			errFunction("Errore in lettura messaggio da bacheca"); 
		}
		printMessage(msgMessage);
	}

	//in che caso potrebbe andare male
	//serve un timeout e mandare command failure o qualcosa
	//SIVULLAPRE QUESTO PUNTO
	if(writeCom(socket, COMMAND_SUCCESS) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	}
}


static void postMessageFunction(int socket)
{
	int ret; 
	char c; 
	char objBuffer[SIZE_OBJECT +1]; 
	char textBuffer[SIZE_TEXT +1]; 
	
	char msgMessage[SIZE_MSG_MESSAGE];
	
	printf("Inserire oggetto del messaggio (massimo 64 caratteri) :\n"); 
	if(getInput(objBuffer, SIZE_OBJECT+1)){
		errFunction("Errore di lettura oggetto del messaggio da stdin"); 
	}

	printf("Inserire testo del messaggio (massimo 160 caratteri) :\n"); 
	if(getInput(objBuffer, SIZE_OBJECT+1)){
		errFunction("Errore di lettura testo del messaggio da stdin\n"); 
	}

	padBuff(objBuffer, strlen(objBuffer), SIZE_OBJECT +1); 
	padBuff(textBuffer, strlen(textBuffer), SIZE_TEXT +1); 
	buildMessage(msgMessage, objBuffer, textBuffer, SIZE_MSG_MESSAGE); 
	
	//la write è da mettere apposto comunque
	
	write(socket,msgMessage, sizeof(msgMessage)); 


	
	ret = readCom(socket, &c); 

	if(ret == 0){
		errFunction("Connessione chiusa dal peer"); 
	}else if(ret == -1){
		errFunction("Errore di lettura dalla socket"); 
	}	
	
	//eeehehehe solito problema ma io faccio funzioni che ritornano erri ma quando sbagli chi oh e come brro non lo so forse non curo l'errore dentro.. non lo so ma se manda roba non riconosciuta
	switch(c){
		case COMMAND_SUCCESS: 
			printf("Messaggio registrato con successo !!"); 
			break; 
		case COMMAND_FAILURE: 
			printf("Impossibile registrare messaggio"); 
			break; 
		default: 
			printf("NOn so cosa fare aiuto\n");  
	}
}





void clientFunc(int socket)
{

	int ret; 
	char c; 
	char username[SIZE_USERNAME+1]; 
	char password[SIZE_PASSWORD+1];
	
	ret = readCom(socket, &c); 
	
	if(ret == 0){
		errFunction("Connessione chiusa dal server"); 
	}else if(ret == -1){
		errFunction("Errore di lettura dala socket"); 
	}
	
	if( c == COMMAND_AUTH ) {
		authFunctionClient(username, password, socket); 
	}else{
		printf("Il codice usato è %d\n", COMMAND_AUTH); 
		errFunction("Errore di comunicazione1"); 
	}
	
	while(1){
		printf("Menu\n"); 
		printf("1:Inserire un nuovo messaggio\n"); 
		printf("2:Visualizzare tutti i messaggi\n"); 
		printf("3:Eliminare un messaggio\n"); 
		printf("4:Quit\n"); 
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
			case '4': 
				writeCom(socket, COMMAND_QUIT); 
				exit(EXIT_SUCCESS); 
			default: 
				printf("Opzione del menu non riconosciuta"); 
		}
	}
	
	
}
