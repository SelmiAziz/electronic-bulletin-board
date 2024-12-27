#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "../include/utilityForFunctionServer.h"

void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen)
	{
		memset(buffer + len, ' ', maxLen-len); 
	}
	buffer[maxLen] = 0; 
}

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

int findEndField(char *f, int endMax)
{ 
	for(int i = endMax; i >= 0; i--)
	{
		if(f[i] == ' ') return i; 
	}
	return endMax; 

}


void extractField(char *s,char *str,  int lStart, int lMax)
{
	int i ; 
	int l = 0; 
	int lEnd = findEndField(s,lMax); 
	for( i = lStart; i < lEnd && !(s[i] == ' ' && s[i+1] == ' '); i++)
	{
		//per adesso mi accontento di questo ma questa funazione è da migliorare perché deve prendere tutta la stringa fino a quando effettiviamente stanno solo spazi
		str[l] = s[i]; 
		l++;
	}
	str[l] = '\0'; 
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

