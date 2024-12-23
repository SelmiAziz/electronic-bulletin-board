#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "../include/helper.h"


void errFunction(char *err){
	fprintf(stderr, err); 
	exit(EXIT_FAILURE); 
}

int getInput(char *buffer, int maxLen){
	if(fgets(buffer, maxLen, stdin)){
		buffer[strcspn(buffer, "\n")] = 0;
		return 0;  
	}else{
		return -1;
	}
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
