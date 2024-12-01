#include<unistd.h>

#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "helper.h"

#define SIZE 1024
#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64

void errFunction(char *err){
	fprintf(stderr, err); 
	exit(EXIT_FAILURE); 
}


//this function is for the client
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


//parte delle funzioni che implementerò magari servono al client oppure al server
int getInput(char *buffer, int maxLen){
	if(fgets(buffer, maxLen, stdin)){
		buffer[strcspn(buffer, "\n")] = 0;
		return 0;  
	}else{
		return -1;
	}
}

//name is awful i know
void fill(char *bufferLine, char *buffUsername, char *buffPassword){
	char buffTemp[SIZE];
	char *token; 
	int len;  
	
	strncpy(buffTemp, bufferLine, SIZE -1); 
	buffTemp[SIZE-1] = '\0';
	
	token = strtok(buffTemp,"\","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	len = strlen(token); 
	if( len > SIZE_USERNAME)
	{
		errFunction("Errore dimensione username del file non corrisponde alla dimensione effettiva"); 
	}
	strncpy(buffUsername, token, len); 
	buffUsername[len] = '\0';
	
	token = strtok(NULL, "\","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	len = strlen(token); 
	if( len > SIZE_PASSWORD)
	{
		errFunction("Errore dimensione password del file non corrisponde alla dimensione effettiva"); 
	}
	strncpy(buffPassword, token, len); 
	buffPassword[len] = '\0';
}

void safeCopy(char *dest, const char *src) {
	if (src != NULL && dest != NULL) {
    	if (*src == '"') {
        	src++;
         }
         size_t len = strlen(src);
         if (len > 0 && src[len - 1] == '"') {
         	len--;
         }
         strncpy(dest, src, len);
         dest[len] = '\0';
        }
}

void fillMsg(char *l, char *buffUser, char *buffObj, char *buffText, char *buffMessageId, int *v) {
    char *token;

    token = strtok(l, ",");
    safeCopy(buffObj, token);

    token = strtok(NULL, ",");
   	safeCopy(buffText, token);

    token = strtok(NULL, ",");
   safeCopy(buffUser, token);

    token = strtok(NULL, ",");
    safeCopy(buffMessageId, token);

    token = strtok(NULL, ",");
    if (token != NULL) {
        *v = strtol(token, NULL, 10);
    }
}



//write file for message and for user

int wrUser(char *buffUser, char *buffPassword, char *file){
	char buff[SIZE_BUFF]; 
	FILE *myFile = fopen(file, "a"); 
	if(myFile == NULL){
		fprintf(stderr, "Error in opening file\n"); 
		exit(EXIT_FAILURE); 
	}
	sprintf(buff, "\"%s\",\"%s\"\n", buffUser, buffPassword); 
	fprintf(myFile, "%s", buff); 
	fclose(myFile); 
	return 0; 
}


//passandola sotto va a scrive direttamente cercare di fare delle modifiche
void wrMessage(char*buffUser, char *buffObj, char *buffText, char *buffIdMessage, char *file){
	char buff[SIZE_BUFF]; 
	FILE *myFile = fopen(file, "a"); 
	if(myFile == NULL){
		fprintf(stderr, "Error in opening file\n");
		exit(EXIT_FAILURE); 
	}
	sprintf(buff, "\"%s\",\"%s\",\"%s\",\"%s\",1\n", buffObj, buffText, buffUser,buffIdMessage); 
	fprintf(myFile,buff); 
	fclose(myFile); 
}

