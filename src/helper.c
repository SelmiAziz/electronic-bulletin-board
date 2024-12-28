#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "../include/helper.h"

// This function handles errors by printing the error message and exiting the program.
void errFunction(char *err){
    fprintf(stderr, "%s",err); 
    exit(EXIT_FAILURE); 
}

// This function reads input from stdin into the provided buffer and removes the newline character at the end.
int getInput(char *buffer, int maxLen){
    if(fgets(buffer, maxLen, stdin)){
        buffer[strcspn(buffer, "\n")] = 0;
        return 0;  
    }else{
        return -1;
    }
}

// This function converts a string to an integer. It returns -1 on failure or the integer value on success.
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

// This function removes quotes (") from a string.
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


int findEndField(char *f, int lStart, int endMax)
{ 
	for(int i = endMax; i >= lStart; i--)
	{
		if(f[i] == ' ') return i; 
	}
	return endMax; 

}


void extractField(char *s,char *str,  int lStart, int lMax)
{
	int i ; 
	int l = 0; 
	int lEnd = findEndField(s,lStart, lMax); 
	for( i = lStart; i < lEnd && !(s[i] == ' ' && s[i+1] == ' '); i++)
	{
		str[l] = s[i]; 
		l++;
	}
	str[l] = '\0'; 
}


void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen)
	{
		memset(buffer + len, ' ', maxLen-len); 
	}
	buffer[maxLen] = 0; 
}

