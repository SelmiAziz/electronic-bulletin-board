#include<unistd.h>

#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "helper.h"

#define SIZE 1024
#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64

#define SIZE_OBJECT 64 
#define SIZE_TEXT 160

#define SIZE_ID_MESSAGE 6

void errFunction(char *err){
	fprintf(stderr, err); 
	exit(EXIT_FAILURE); 
}

void removeQuotes(char *str) 
{
	char *src = str, *dst = str; 
	
	while(*src)
	{
		if(*src != '"')
		{
			*dst++ = *src;
		}
		src++;
	}
	*dst = '\0'; 

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


void copyToken(char *destBuff,char *strToken, int lenMax)
{
	int len = strlen(strToken); 

	if(len > lenMax)
	{
		errFunction("Errore dimensione campo non corrisponde a quell'effettiva"); 
	
	}
	strncpy(destBuff,strToken, len); 

	destBuff[len] = '\0'; 

}

void copyTokenWithoutQuotes(char *destBuff,char *strToken, int lenMax)
{
	char buffTemp[lenMax]; 
	int len = strlen(strToken); 

	if(len > lenMax)
	{
		errFunction("Errore dimensione campo non corrisponde a quell'effettiva"); 
	
	}
	strncpy(buffTemp,strToken, len); 
	removeQuotes(buffTemp); 
	
	len = len-2; 

	strncpy(destBuff, buffTemp, len); 
	destBuff[len] = '\0'; 

}

//name is awful i know i named it filleUser
void fill(char *buffLine, char *buffUsername, char *buffPassword){
	char buffTemp[SIZE];
	char *token; 
	int len;  
	
	strncpy(buffTemp, buffLine, SIZE -1); 
	buffTemp[SIZE-1] = '\0';
	
	token = strtok(buffTemp,"\","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	copyToken(buffUsername, token, SIZE_USERNAME); 
	
	token = strtok(NULL, "\","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	copyToken(buffPassword, token, SIZE_PASSWORD); 
}


int convertStringToNumber(char *str)
{
	int n;
	char *endptr; 
	int success; 
	
	n = (int) strtol(str, &endptr, 10); 
	
	if(errno == ERANGE)
	{
		success = -1; 
	}else if(str == endptr)
	{
		success = -1;
	}else{
		success = 0;
	}
	
	if(success != 0) return success; 
	return n; 

}




void fillMsg(char *buffLine, char *buffUsername, char *buffObject, char *buffText, char *buffIdMessage, int *presenceBit) {

	char buffTemp[SIZE];
	char *token; 
	int len;  
 
 
	strncpy(buffTemp, buffLine, SIZE -1); 
	buffTemp[SIZE-1] = '\0';

	token = strtok(buffTemp, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	copyTokenWithoutQuotes(buffObject, token, SIZE_OBJECT+2); 
	
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	copyTokenWithoutQuotes(buffText, token, SIZE_TEXT+2);  
	
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	copyTokenWithoutQuotes(buffUsername, token, SIZE_USERNAME+2); 
	
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	copyTokenWithoutQuotes(buffIdMessage, token, SIZE_ID_MESSAGE+2); 
	
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	*presenceBit = convertStringToNumber(token); 
	if(*presenceBit == -1)
	{
		errFunction("Errore di conversione flag di presenza");
	}
}



void wrUser(char *buffUser, char *buffPassword, char *file){
	char buff[SIZE]; 
	
	FILE *myFile = fopen(file, "a"); 
	if(myFile == NULL)
	{
		errFunction("Errore in apertura file"); 
	}
	
	sprintf(buff, "\"%s\",\"%s\"\n", buffUser, buffPassword); 
	fprintf(myFile, "%s", buff); 
	fclose(myFile); 
}



void wrMessage(char*buffUser, char *buffObj, char *buffText, char *buffIdMessage, char *file){
	char buff[SIZE];
	
	FILE *myFile = fopen(file, "a"); 
	if(myFile == NULL)
	{
		errFunction("Errore in apertura file\n"); 
	}
	sprintf(buff, "\"%s\",\"%s\",\"%s\",\"%s\",1\n", buffObj, buffText, buffUser,buffIdMessage); 
	fprintf(myFile,buff); 
	fclose(myFile); 
}

