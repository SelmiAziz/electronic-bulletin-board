#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include "../include/utilityForFunctionClient.h"
#include "../include/commFieldsConfig.h"
#include "../include/functionClient.h"
#include "../include/helper.h"
#include "../include/protocolUtilis.h"




int checkFormatUsername(char *username)
{
	if(strlen(username) > 4) return 0; 
	return -1; 

}

void getValideUsername(char *username)
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


int controlPass(char *password, char *seq){
	for(int i = 0; i<strlen(password); i++){
		for(int j = 0; j<strlen(seq); j++){
			if(password[i] == seq[j]){
				return 0; 
			}
		}
	}
	return -1; 

}

int checkFormatPassword(char *password){
	char *specials = "!?#$%^&*()-+_="; 
	char *num = "1234567890"; 
	char *alM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	if(strlen(password) < 8) return -1; 
	if(controlPass(password, specials) || controlPass(password,num) || controlPass(password, alM)) return -1;
	return 0; 


}

void getValidePassword(char *password){
	while(1){
		if(getInput(password, SIZE_PASSWORD+1))
		{
			errFunction("Errore di lettura password da stdin");
		}
		if(checkFormatPassword(password) == 0) break; 
		printf("Formato password :almeno 8 caratteri tra cui una lettera maiuscola, un numero e un carattere speciale\n"); 
	}
}



void buildMessage(char *bMessage, char *s1, char *s2, int len)
{
	sprintf(bMessage, "%s%s", s1, s2); 
}

int numberMessages(int socket)
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
	
	extractField(numMsgBuff,numMsgBuff,0,SIZE_NUM_MSG); 
	
	
	
	n = strtol(numMsgBuff, NULL, 10);


	if(n > 0) return n; 
	return -1; 

}

