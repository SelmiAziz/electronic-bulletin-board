#include<unistd.h>

#include<errno.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include "../include/functionClient.h"
#include "../include/helper.h"
#include "../include/protocolUtilis.h"

//this function has to be only for the client part
int takeArgumentsClient(int argc, char **argv, char **serverAddress, short int *port){
	int i = 0; 
	char *endptr; 
	
	if(argc != 5)
	{
		//magari cambiare per avere un'usciata con EXIT_SUCCESS
		errFunction("Sintassi: eseguibile a indirizzo p porta"); 
	}
	
	while(i < argc -1)
	{
		if(strncmp(argv[i+1], "a", 1) == 0)
		{
			*serverAddress = argv[++i+1];
		}
		else if(strncmp(argv[i+1], "p",1) == 0)
		{
			*port = strtol(argv[++i+1], &endptr, 10); 
			if(*endptr)
			{
				errFunction("Porta non riconosciuta");
			}
		}
		i++; 	
	}
	return 0; 
}



static int readBuffSocket(int fd, void *buffer, size_t total_bytes)
{
    size_t bytes_read = 0;
    ssize_t result;
    int attempts = 0;

    while (bytes_read < total_bytes) {
        result = read(fd, (char *)buffer + bytes_read, total_bytes - bytes_read);

        if (result < 0) {
            return -1; 
        } else if (result == 0) {
            return 0; 
        }

        bytes_read += result;
    }

    return bytes_read; 
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


void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen)
	{
		memset(buffer + len, ' ', maxLen-len); 
	}
	buffer[maxLen] = 0; 
}

void buildMessage(char *bMessage, char *s1, char *s2, int len)
{
	sprintf(bMessage, "%s%s", s1, s2); 
}

int findEndField(char *f, int lStart, int endMax)
{ 
	for(int i = endMax; i >= lStart; i--)
	{
		if(f[i] == ' ') return i; 
	}
	return endMax; 

}


void extractField(char *s,char *str,  int lStart, int lMax)
{
	int i ; 
	int l = 0; 
	int lEnd = findEndField(s,lStart, lMax); 
	for( i = lStart; i < lEnd && !(s[i] == ' ' && s[i+1] == ' '); i++)
	{
		//per adesso mi accontento di questo ma questa funazione è da migliorare perché deve prendere tutta la stringa fino a quando effettiviamente stanno solo spazi
		str[l] = s[i]; 
		l++;
	}
	str[l] = '\0'; 
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
		
		padBuff(username, strlen(username), SIZE_USERNAME); 
		padBuff(password, strlen(password), SIZE_PASSWORD); 
		
		buildMessage(authMessage, username, password,SIZE_AUTH_MESSAGE); 
   		
   		//sviluppare una write che funzioni
   		//sostituzione qui
   		if(writeBuffSocket(socket, authMessage, SIZE_AUTH_MESSAGE) == -1)
   		{
   			errFunction("Errore di scrittura sulla socket"); 
   		}
	 
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
		
 
		padBuff(username, strlen(username), SIZE_USERNAME);
		padBuff(password, strlen(password), SIZE_PASSWORD);
   
   		buildMessage(authMessage, username, password,SIZE_AUTH_MESSAGE); 
   		
   		//sviluppare una write che funzioni
   	
   		if(writeBuffSocket(socket, authMessage, SIZE_AUTH_MESSAGE) == -1)
   		{
   			errFunction("Errore di scrittura sulla socket"); 
   		} 
   		
   		ret = readCom(socket, &c);
   	
   		
		if(ret == 0){
			errFunction("Connessione chiusa dal server"); 
		}else if(ret == -1){
			errFunction("Errore di lettura dalla socket"); 
		}		
		
		switch(c){
			case COMMAND_SUCCESS: 
				printf("Registrazione andata a buon fine\n"); 
				fflush(stdout); 
				break;
			case COMMAND_ERR_USER_ALREADY_EXISTS: 
				printf("L'utente è già registrato nel sistema\n"); 
				fflush(stdout); 
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
		if(getInput(buffer, SIZE))
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
	int ret; 
	char c; 
	char idMessage[SIZE_ID_MESSAGE+1]; 
	
	if( writeCom(socket, COMMAND_DELETE_MSG) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	}

	printf("Scrivi Id da eliminare\n"); 
	if(getInput(idMessage, SIZE_ID_MESSAGE+1) == -1)
	{
		errFunction("Errore di lettura id messagio da eliminare"); 
	}
	while(getchar() != '\n'); 

	if (writeBuffSocket(socket, idMessage, SIZE_ID_MESSAGE) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	
	}
		
	ret = readCom(socket, &c) ; 
	//una parte del mio codice mi dice di fare così ma sono sicuro di aver controllato il ret == 0 da tutte le parti ?? 
	if(ret == 0){
		errFunction("Connessione chiusa dal peer"); 
	}else if(ret == -1){
		errFunction("Errore di lettura dalla socket"); 
	}	
	
	switch(c){
		case COMMAND_FAILURE: 
			printf("Eliminazione del messaggio andata male");
			break; 
		case COMMAND_SUCCESS: 
			printf("Eliminazione del messaggio andata a buon fine");
			break; 
		default: 
			printf("Non riconosciuta"); 
	}
}

static void postMessageFunction(int socket)
{
	int ret; 
	char c; 
	char objBuffer[SIZE_OBJECT+1]; 
	char textBuffer[SIZE_TEXT+1]; 
	
	char msgMessage[SIZE_MSG_MESSAGE];
	
	
	if(writeCom(socket, COMMAND_POST_MSG) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	}
	
	printf("Inserire oggetto del messaggio (massimo 64 caratteri) :\n"); 
	if(getInput(objBuffer, SIZE_OBJECT+1)){
		errFunction("Errore di lettura oggetto del messaggio da stdin"); 
	}

	printf("Inserire testo del messaggio (massimo 160 caratteri) :\n"); 
	fflush(stdout);
	if(getInput(textBuffer, SIZE_TEXT+1)){
		errFunction("Errore di lettura testo del messaggio da stdin\n"); 
	}

	padBuff(objBuffer, strlen(objBuffer), SIZE_OBJECT ); 
	padBuff(textBuffer, strlen(textBuffer), SIZE_TEXT ); 
	buildMessage(msgMessage, objBuffer, textBuffer, SIZE_MSG_MESSAGE); 
	
	
	
	if((ret = writeBuffSocket(socket,msgMessage, SIZE_MSG_MESSAGE)) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	}


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
			fflush(stdout); 
			break; 
			//non ho idea di cosa faccia questa roba
		case COMMAND_FAILURE: 
			printf("Impossibile registrare messaggio"); 
			fflush(stdout);
			break; 
		default: 
			printf("Non so cosa fare aiuto\n");  
			fflush(stdout);
	}
}

static void printGenericMessage(char *message)
{

	char objBuffer[SIZE_OBJECT+1]; 
	char textBuffer[SIZE_TEXT+1]; 
	char idBuffer[SIZE_ID_MESSAGE+1]; 
	char authorBuffer[SIZE_USERNAME+1]; 
	//sarebbe meglio mandare anche un codice con il messaggio in modo da indentificarlo
	//STA ROBA NON MI PIACE
	extractField(message,objBuffer,0, SIZE_OBJECT); 
	extractField(message,textBuffer, SIZE_OBJECT, SIZE_OBJECT + SIZE_TEXT); 
	extractField(message,idBuffer,SIZE_OBJECT + SIZE_TEXT, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE); 
	extractField(message,authorBuffer, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE + SIZE_USERNAME); 
	
	
	printf("\n\nCarattere\n"); 
	
	printf("Il carattere %c\n", message[SIZE_OBJECT + SIZE_TEXT+1]);
	
	printf("\n"); 
	//should slplit the message
	
	printf("<-----Messaggio----->"); 
	printf("\n"); 
	printf("Oggetto:%s",objBuffer); 
	printf("\n"); 
	printf("Testo:%s",textBuffer); 
	printf("\n"); 
	printf("ID Messaggio:%s",idBuffer);; 
	printf("\n"); 
	printf("Autore: %s", authorBuffer);
	printf("\n"); 
	printf("<------------------->"); 



}

static void printPersonalMessage(char *message)
{

	char objBuffer[SIZE_OBJECT+1]; 
	char textBuffer[SIZE_TEXT+1]; 
	char idBuffer[SIZE_ID_MESSAGE+1]; 
	//sarebbe meglio mandare anche un codice con il messaggio in modo da indentificarlo
	extractField(message,objBuffer,0, SIZE_OBJECT); 
	extractField(message,textBuffer, SIZE_OBJECT, SIZE_OBJECT + SIZE_TEXT); 
	extractField(message,idBuffer,SIZE_OBJECT + SIZE_TEXT, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE); 
	
	printf("\n\nCarattere\n"); 
	
	printf("Il carattere %c\n", message[SIZE_OBJECT + SIZE_TEXT+1]);
	
	printf("\n"); 
	//should slplit the message
	
	printf("<-----Messaggio----->"); 
	printf("\n"); 
	printf("Oggetto:%s",objBuffer); 
	printf("\n"); 
	printf("Testo:%s",textBuffer); 
	printf("\n"); 
	printf("ID Messaggio:%s",idBuffer);; 
	printf("\n"); 
	printf("<------------------->"); 



}




static int numberMessages(int socket)
{
	int ret, n; 
	char numMsgBuff[SIZE_NUM_MSG+1];

	ret = readBuffSocket(socket, numMsgBuff, SIZE_NUM_MSG) ; 
	
	if(ret == 0)
	{
		errFunction("Connessione chiusa dal server"); 	
	}else if( ret == -1)
	{
		errFunction("Errore di lettura dalla socket"); 
	}
	
 
 	//sta funzione è strana in quanto è lo stesso buffer che sovrascrivo
	extractField(numMsgBuff,numMsgBuff,0,SIZE_NUM_MSG); 
	
	
	
	n = strtol(numMsgBuff, NULL, 10);


	if(n > 0) return n; 
	return -1; 



}


static void viewMessageFunction(int socket)
{

	int ret, n; 
	char c; 
	char numMsgBuff[SIZE_NUM_MSG+1];
	char objBuffer[SIZE_OBJECT+1]; 
	char textBUffer[SIZE_TEXT+1]; 
	char idBuffer[SIZE_ID_MESSAGE+1]; 
	char msgMessage[SIZE_PERSONAL_COMPLETE_MESSAGE];
	 

	if(writeCom(socket,COMMAND_VIEW_MSG) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	
	}

	n = numberMessages(socket); 	
	if(n == -1)
	{
		//qua che cosa faccio oh
	}

	for(int i = 0; i<n; i++){
		if(readBuffSocket(socket, msgMessage, SIZE_PERSONAL_COMPLETE_MESSAGE) == -1)
		{
			errFunction("Errore in lettura messaggio da bacheca"); 
		}
		printPersonalMessage(msgMessage); 
	}

	

	//in che caso potrebbe andare male
	//serve un timeout e mandare command failure o qualcosa
	//SIVULLAPRE QUESTO PUNTO
	
	ret = readCom(socket, &c); 

	if(ret == 0){
		errFunction("Connessione chiusa dal peer"); 
	}else if(ret == -1){
		errFunction("Errore di lettura dalla socket"); 
	}	
	//eeehehehe solito problema ma io faccio funzioni che ritornano erri ma quando sbagli chi oh e come brro non lo so forse non curo l'errore dentro.. non lo so ma se manda roba non riconosciuta
	switch(c){
		case COMMAND_SUCCESS: 
			//printf("Messaggio registrato con successo !!"); 
			fflush(stdout); 
			break; 
		default: 
			printf("Non so cosa fare aiuto\n");  
			fflush(stdout);
	}

}






static void viewAllMessageFunction(int socket)
{
	int ret; 
	char c; 
	char msgMessage[SIZE_GENERIC_COMPLETE_MESSAGE];
	int n; 

	if(writeCom(socket,COMMAND_VIEW_ALL_MSG) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	
	}

	n = numberMessages(socket); 


	for(int i = 0; i<n; i++){
		if(readBuffSocket(socket, msgMessage, SIZE_GENERIC_COMPLETE_MESSAGE) == -1)
		{
			errFunction("Errore in lettura messaggio da bacheca"); 
		}
		printGenericMessage(msgMessage); 
	}

	fflush(stdout); 

	//in che caso potrebbe andare male
	//serve un timeout e mandare command failure o qualcosa
	//SIVULLAPRE QUESTO PUNTO
	//if(readCom(socket,&c) == -1)
	//{
		//errFunction("Errore di scrittura sulla socket"); 
	//}
	
	ret = readCom(socket, &c); 

	if(ret == 0){
		errFunction("Connessione chiusa dal peer"); 
	}else if(ret == -1){
		errFunction("Errore di lettura dalla socket"); 
	}	
	//eeehehehe solito problema ma io faccio funzioni che ritornano erri ma quando sbagli chi oh e come brro non lo so forse non curo l'errore dentro.. non lo so ma se manda roba non riconosciuta
	switch(c){
		case COMMAND_SUCCESS: 
			//printf("Messaggio registrato con successo !!"); 
			//fflush(stdout); 
			break; 
		default: 
			printf("Non so cosa fare aiuto\n");  
			fflush(stdout);
	}
	
}





//ATTENZIONE IL CLIENT NON HA ANCORA UNA READ FUNCTION DECENTE SI STA APPOGGIANDO A QUELLA TIMEOUT
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
		printf("\nMenu\n"); 
		printf("1:Inserire un nuovo messaggio\n"); 
		printf("2:Visualizzare messaggi\n"); 
		printf("3:Visualizzare tutti i messagi\n"); 
		printf("4:Eliminare un messaggio\n"); 
		printf("5:Quit\n"); 
		c = getchar(); 
		while(getchar() != '\n'); 
		
		switch(c){
			case '1': 
				postMessageFunction(socket);
				break; 
			case '2': 
				viewMessageFunction(socket);
				//postMessageFunction(socket); 
				break; 
			case '3': 
				viewAllMessageFunction(socket); 
				break; 
			case '4':  
				delMessageFunction(socket); 
				break; 
			case '5': 
				//ma il thread del server che fine fa brother eh capiscilo
				exit(EXIT_SUCCESS); 
			default: 
				printf("Opzione del menu non riconosciuta"); 
		}

	}
	
	
}
