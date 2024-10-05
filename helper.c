#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "helper.h"

void errFunction(char *err){
	fprintf(stderr, err); 
	exit(EXIT_FAILURE); 
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
	printf("Finisih fill\n"); 
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

void wrUser(char *buffUser, char *buffPassword, char *file){
	char buff[1024]; 
	FILE *myFile = fopen(file, "a"); 
	if(myFile == NULL){
		fprintf(stderr, "Error in opening file\n"); 
		exit(EXIT_FAILURE); 
	}
	sprintf(buff, "\"%s\",\"%s\"\n", buffUser, buffPassword); 
	fprintf(myFile, "%s", buff); 
	fclose(myFile); 
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

