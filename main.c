#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "messageLib.h"
#include "helper.h"


void jo(char *buffUser, char *buffPass, char *buffObj, char *buffText, char scelta, User **h){


}

int main(int arcv, char *argv[]){
	char buffUser[MAX_USERNAME]; 
	char buffPass[MAX_PASSWORD];  
	char buffObj[MAX_OBJECT]; 
	char buffText[MAX_TEXT]; 
	char scelta; 
	User *head = NULL; 
	fillUsers(&head, "back_users.csv");
	fillMessagesUsers(head, "back_messages.csv");
	while(1){
	printf("Comandi:\n"); 
			printf("1.Inserisci un utente\n"); 
			printf("2.Inserisci messaggio utente\n"); 
			printf("3.Visualizza messaggi utente\n"); 
			printf("4.Inserisci username e oggetto messaggio eliminare\n"); 
			printf("5.Guarda gli users\n"); 
			scelta = getchar(); 
			while(getchar() != '\n'); 
			
			switch(scelta){
				case '1':
					printf("Inserisci username\n"); 
					getInput(buffUser, MAX_USERNAME); 
					printf("Inserisci password\n"); 	
					getInput(buffPass, MAX_PASSWORD); 
					addUser(&head, buffUser, buffPass); 
					wrUser(buffUser, buffPass, "back_users.csv"); 
					break; 
				case '2': 
					printf("Inserisci utente\n"); 
					getInput(buffUser, MAX_USERNAME); 
					printf("Inserisci oggetto\n"); 
					getInput(buffObj, MAX_OBJECT); 
					printf("Inserisci testo\n"); 
					getInput(buffText, MAX_TEXT); 
					addMessageUser(head, buffUser, buffObj, buffText); 
					wrMessage(buffUser, buffObj, buffText, "back_messages.csv"); 
					break; 
				case '3': 	
					printf("Inserisci utente\n"); 
					getInput(buffUser, MAX_USERNAME); 
					printUserMessage(head, buffUser); 
					break; 
				case '4': 
					printf("Inserisci utente\n"); 
					getInput(buffUser, MAX_USERNAME); 
					printf("Inserisci oggetto\n"); 
					getInput(buffObj, MAX_OBJECT); 
					delMessageUser(head, buffUser, buffObj); 
					//serve una funzione di delit
					break; 
				case '5': 
					visualizeUsers(head); 
					break; 
					
					
				default:
					printf("Err\n"); 
			
			}

 
	}
	

	return 0; 
}
