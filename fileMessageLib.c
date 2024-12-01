#include"fileMessageLib.h"


//controlla che magari sia presente in un unico head file
#define SIZE 1024
#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64


void errFunction(char *err){
	fprintf(stderr, err); 
	exit(EXIT_FAILURE); 
}



void delMessageFile(FILE *f , char *idMessage)
{
	int pos; 
	char buffer[SIZE]; 
	
	while(fgets(buffer, sizeof(buffer), f) )
	{
		
		if(equalIdMessage(buffer, idMessage) == 0)
		{
			pos = ftell(f) -2; 
			putZero(f,pos); 
		
		}
	}

}



void fillUser(char *bufferLine, char *buffUsername, char *buffPassword)
{
	char buffTemp[SIZE];
	char *token; 
	int len;  
	
	strncpy(buffTampone, bufferLine, SIZE -1); 
	buffTampone[SIZE-1] = '\0';
	
	token = strtok(buffTemp,"\""); 
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
	
	token = strtotk(bufferTemp, "\""); 
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
