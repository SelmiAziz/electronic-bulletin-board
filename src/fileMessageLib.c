#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>

#include "../include/fileMessageLib.h"
#include "../include/helper.h"
#include "../include/commServerClientConfig.h"

#define SIZE 1024


//ausiliary function to fillMsg, it is directly connected to how the messages are rappresentend in the csv back_up file
void static copyTokenWithoutQuotes(char *destBuff,char *strToken, int lenMax)
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

//this function takes a line from a file and converts it in temporarly buffers, that will be used to populate the electronic bulletin board in memory.
void  fillMsg(char *buffLine, char *buffUsername, char *buffObject, char *buffText, char *buffIdMessage, int *presenceBit) {

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

//ausiliary function to fillUser, it is directly connected to how the messages are rappresentend in the csv back_up file
void static copyToken(char *destBuff,char *strToken, int lenMax)
{
	int len = strlen(strToken); 

	if(len > lenMax)
	{
		errFunction("Errore dimensione campo non corrisponde a quell'effettiva"); 
	
	}
	strncpy(destBuff,strToken, len); 

	destBuff[len] = '\0'; 

}


//this functonc will take the contenuto of a line of the file of back_up and will convert it into temporarly buffers, that will be used to populate the elcectronic bulletin board in memory
void fillUser(char *bufferLine, char *buffUsername, char *buffPassword)
{
	char buffTemp[SIZE];
	char *token; 
	int len;  
	
	strncpy(buffTemp, bufferLine, SIZE -1); 
	buffTemp[SIZE-1] = '\0';
	
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
	
	strtok(NULL, "\""); 
	
	token = strtok(NULL, "\""); 
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

//ausiliary function to delMessageFile, it checks if the current line of the file examinated rappresents the message that the user wants to eliminate
//it will return 0 if the filed Id Messaged matches the idMessage provided by the user
//it will return -1 instead
int static equalIdMessage(char *buffer, char *idMessage)
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

//in a file it will convert the character 1 to zero in the position pos
void static zeroPresence(FILE *f, int pos)
{
	if (fseek(f, pos, SEEK_SET) != 0) {
            perror("Errore con fseek");
            fclose(f);
        }
        
        //when something goes wrong fputc returns EOF 
        if (fputc('0', f) == EOF) {
            perror("Errore nella scrittura del carattere");
            fclose(f);
        }
       fseek(f, pos+2, SEEK_SET); 

}


//this function will eliminate a message, in reality the messages will not be eliminated immediately, instead the presence bit will be put to zero, in order to eliminate it in a second moment
void delMessageFile(char *file , char *idMessage)
{
	int pos; 
	char buffer[SIZE]; 
	FILE *f = fopen(file,"r+"); 
	if(f == NULL)
	{
		errFunction("Errore apertura file"); 
	}
	
	while(fgets(buffer, sizeof(buffer), f) )
	{
		
		if(equalIdMessage(buffer, idMessage) == 0)
		{
			pos = ftell(f) -2; 
			zeroPresence(f,pos); 
		
		}
	}
	fclose(f);
}

//this function will write in a line of file a user that has been registreted in the system
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


//this functon will write in a line of file a message that has been registreted in the system
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

//this function will copy a content of fileSource to fileDestination
void copyFile(const char* fileSource, const char* fileDestination)
{
    FILE *source, *destination;
    char buffer[1024]; 
    int bytesRead; //maybe size_t

    source = fopen(fileSource, "r");
    if (source == NULL) {
       	errFunction("Errore nell'apertura file sorgente"); 
    }

    destination = fopen(fileDestination, "w");
    if (destination == NULL) {
        fclose(source);
        errFunction("Errore nell'apertura file destinazione");
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
        fwrite(buffer, 1, bytesRead, destination);
    }

    fclose(source);
    fclose(destination);
}

//ausiliary function to eliminateZeroPresence
//it will return 0 if the current buffer that contains a line of the back_up file in the field presence bit contains 0
//otherwise il will return -1
int static checkZeroPresence(char *buffer)
{
	char *pt = strdup(buffer); 
	char *token = strtok(pt, ","); 
	char *last = NULL; 
	
	while(token)
	{	
		last = token; 
		token = strtok(NULL, ","); 
	
	}

	if(*last ==  '0') return 0; 

	free(pt); 
	pt = NULL; 
	return -1; 

}

//this function will eliminate from a file all the messages that have the presence bit to zero
//maybe I should put the copyfunction directly in this function
void eliminateZeroPresence(char *file, char *fileShadow)
{
	char buffer[SIZE]; 
	
	FILE *f = fopen(file, "r+");
	if(f == NULL)
	{
		errFunction("Errore nell'apertura file");
	}
	
	FILE *newf = fopen(fileShadow, "w+"); 
	if(newf == NULL)
	{
		errFunction("Errore nell'apertura file");  
	}
	
	rewind(f); 
	while(fgets(buffer, SIZE, f))
	{
		if(checkZeroPresence(buffer) != 0)
		{
			fprintf(newf, "%s", buffer); 
		}
	
	}
	fflush(newf);
	fclose(newf); 

}


