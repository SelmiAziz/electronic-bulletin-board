#include"fileMessageLib.h"


//controlla che magari sia presente in un unico head file
#define SIZE 1024
#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64

//QUESTA FUNZIONE NON DOVREBBE STARE QUI
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

//QUESTA FUNZIONE DI CONVERSIONE NUMERICA

int convertStringToNumber(char *str)
{
	int n;
	char *endptr; 
	int success; 
	
	n = (int) strtol(buffer, &endptr, 10); 
	
	if(errno == ERRANGE)
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

void fillMsg(char *bufferLine, char *buffUsername,  char *buffObject, char *buffText, char *buffIdMessage, int *flagV)
{
	char buffTemp[SIZE];
	char *token; 
	int len;  
	
	strncpy(buffTampone, bufferLine, SIZE -1); 
	buffTampone[SIZE-1] = '\0';

	token = strtok(buffTemp, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	removeQuotesCopy(bufferObject, token); 
	
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	removeQuotesCopy(bufferText, token); 
		
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	removeQuotesCopy(buffUsername, token); 
	
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	removeQuotesCopy(buffIdMessage, token); 
	
	
	token = strtok(NULL, ","); 
	if(token == NULL)
	{
		errFunction("Errore nessun token"); 
	}
	*v = convertStringToNumber(token); 
	if(*v == -1)
	{
		errFunction("Errore di conversione flag di presenza");
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
