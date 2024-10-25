#include<unistd.h>

#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "helper.h"



void errFunction(char *err){
	fprintf(stderr, err); 
	exit(EXIT_FAILURE); 
}

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
void fill(char *l, char *buffUser, char *buffPass){
	char *token = strtok(l, "\""); 
	strncpy(buffUser, token, strlen(token)); 
	buffUser[strlen(token)] = 0; 
	token = strtok(NULL, "\","); 
	strncpy(buffPass, token, strlen(token)); 
	buffPass[strlen(token)] = 0; 
}

void fillMsg(char *l, char *buffUser, char *buffObj, char *buffText, int *v) {
    char *token;

    // Estrarre l'oggetto
    token = strtok(l, "\"");
    strncpy(buffObj, token, strlen(token));
    buffObj[strlen(token)] = '\0'; // Terminare la stringa

    // Estrarre il testo
    token = strtok(NULL, "\",");
    strncpy(buffText, token, strlen(token));
    buffText[strlen(token)] = '\0'; // Terminare la stringa

    // Estrarre l'autore
    token = strtok(NULL, "\",");
    strncpy(buffUser, token, strlen(token));
    buffUser[strlen(token)] = '\0'; // Terminare la stringa

    // Estrarre la flag di validità
    token = strtok(NULL, ",");
    *v = strtol(token, NULL, 10); // Convertire la flag in intero
}

//write file for message and for user

int wrUser(char *buffUser, char *buffPassword, char *file){
	char buff[1024]; 
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
void wrMessage(char*buffUser, char *buffObj, char *buffText, char *file){
	char buff[1024]; 
	FILE *myFile = fopen(file, "a"); 
	if(myFile == NULL){
		fprintf(stderr, "Error in opening file\n");
		exit(EXIT_FAILURE); 
	}
	sprintf(buff, "\"%s\",\"%s\",\"%s\",1\n", buffObj, buffText, buffUser); 
	fprintf(myFile,buff); 
	fclose(myFile); 
}

