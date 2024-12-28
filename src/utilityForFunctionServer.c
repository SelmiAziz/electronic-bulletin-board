#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "../include/utilityForFunctionServer.h"


void extract2Message(char *s, char *str1, char *str2, int l1, int l2) 
{
    int i, l = 0; 
  
    for(i = 0; i < l1 && !(s[i] == ' ' && s[i+1] == ' '); i++)
    {
        str1[i] = s[i]; 
    }
    str1[i] = '\0'; 

    
  	for(i = l1 ; i < l1 + l2 && !(s[i] == ' ' && s[i+1] == ' ') ; i++)
  	{
 		str2[l] = s[i]; 
 		l++; 
  	}
  	str2[l] = '\0'; 
}


void buildPersonalUserMessage(char *msgMessage, char *object, char *text, char *idMessage)
{
	sprintf(msgMessage,"%s%s%s",object,text,idMessage);
}


void buildGenericUserMessage(char *msgMessage, char *object, char *text, char *idMessage, char *author)
{
	sprintf(msgMessage,"%s%s%s%s",object,text,idMessage,author);
}


void copyInBuffer(char *buff, char *origin)
{
	strncpy(buff, origin, strlen(origin)); 
	buff[strlen(origin)] = 0; 

}

