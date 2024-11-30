#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define SIZE 1024

void remove_quotes(char *str) {
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

void putZero(FILE *f, int pos)
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

int equalMessage(char *buffer, char *idMessage)
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
			
		remove_quotes(pen); 


	if(strcmp(idMessage, pen) == 0 )
	{
		return 0; 
	
	}
	return -1; 

}


void putZeroMain(FILE *f , char *idMessage)
{
	int pos; 
	char buffer[SIZE]; 
	
	while(fgets(buffer, sizeof(buffer), f) )
	{
		
		if(equalMessage(buffer, idMessage) == 0)
		{
			pos = ftell(f) -2; 
			putZero(f,pos); 
		
		}
	}

}

int controlRow(char *buffer)
{
	char *pt = strdup(buffer); 
	char *token = strtok(pt, ","); 
	char *last = NULL; 
	
	while(token)
	{	
		last = token; 
		token = strtok(NULL, ","); 
	
	}

	if(strcmp(last , "0") == 0) return 0; 
	
	free(pt); 
	pt = NULL; 
	return -1; 

}

void eliminateAllZeroRows(FILE *f)
{
	char buffer[SIZE]; 
	
	FILE *newf = fopen("file_shadow.csv", "w+"); 
	
	rewind(f); 
	while(fgets(buffer, SIZE, f))
	{
		printf("Ho letto %s\n", buffer); 
		if(controlRow(buffer) != 0)
		{
			printf("sto scrivendo %s", buffer); 
			fprintf(newf, "%s\n", buffer); 
		}
	
	}
	fflush(newf);
	fclose(newf); 

}

int main(int arcv, char *argv)
{

	FILE * f = fopen("back_messages.csv", "r+"); 

	putZeroMain(f, "000003"); 
	
	
	eliminateAllZeroRows(f); 
	
	
	fclose(f); 

	return 0; 
}
