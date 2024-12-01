#include"fileMessageLib.h"


//controlla che magari sia presente in un unico head file
#define SIZE 1024
#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64

#define SIZE_OBJECT 64 
#define SIZE_TEXT 160

//QUESTA FUNZIONE NON DOVREBBE STARE QUI
void errFunction(char *err){
	fprintf(stderr, err); 
	exit(EXIT_FAILURE); 
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


int equalIdMessage(char *buffer, char *idMessage)
{
		char *token = strtok(buffer,",");
		char *pen = NULL; 
		char *last = NULL; 
		
		
		while(token)
		{
			pen = last; 
			last = token; 
			token = strtok(NULL, ","); 
		
		}
		removeQuotes(pen); 


	if(strcmp(idMessage, pen) == 0 )
	{
		return 0; 
	
	}
	return -1; 

}

//this goes to a separate file
void delMessageFile(FILE *f , char *idMessage)
{
	int pos; 
	char buffer[SIZE]; 
	
	while(fgets(buffer, sizeof(buffer), f) )
	{
		
		if(equalIdMessage(buffer, idMessage) == 0)
		{
			pos = ftell(f) -2; 
			zeroPresence(f,pos); 
		
		}
	}

}


void writeUserFile(char *buffUser, char *buffPassword, char *file){
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



void writeMessageFile(char*buffUser, char *buffObj, char *buffText, char *buffIdMessage, char *file){
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
