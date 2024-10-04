



#include<stdlib.h>
#include<stdio.h>
#include<string.h>




#include"helper.h"


int main(int arcv, char *argv[]){
	char *file = "back_messages.csv"; 
	char buffer[1024]; 
	char user[1024]; 
	char obj[1024]; 
	char text[1024]; 
	int value; 
	FILE *myFile ;
	
	myFile = fopen(file, "r"); 
	if(myFile == NULL){
		fprintf(stderr, "ERror\n"); 
		exit(EXIT_FAILURE); 
	
	}
	fgets(buffer, 1024, myFile); 
	fillMsg(buffer, user, obj, text, &value); 
	printf("Risultato %s %s %s e valore %d", user, obj, text, value); 
	


	return 0; 
}
