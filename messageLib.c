#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"messageLib.h"
#include"helper.h"


int countMessages = 0; 


Message *createMessage(char *object, char *text, char *id)
{
	Message *myMessage = malloc(sizeof(Message)); 
	if(myMessage == NULL){
		fprintf(stderr, "Error in malloc occured!"); 
		exit(EXIT_FAILURE); 
	}
	strncpy(myMessage->object, object, strlen(object)); 
	myMessage->object[strlen(object)] = 0; 
	strncpy(myMessage->text, text, strlen(text)); 
	myMessage->text[strlen(text)] = 0; 
	strncpy(myMessage->idMessage, id, strlen(id)); 
	myMessage->idMessage[strlen(id)] = 0; 
	myMessage->value = 1; 
	return myMessage; 
}


void printMessage(Message *myMessage)
{
	if(myMessage){
		printf("Object is %s text is %s\n", myMessage->object, myMessage->text); 
	}
}



User *createUser(char *username, char *password)
{
	User *myUser = malloc(sizeof(User)); 
	if(myUser == NULL){
		fprintf(stderr, "Error in malloc occured!"); 
		exit(EXIT_FAILURE); 
	}

	strncpy(myUser->username, username, strlen(username)); 
	myUser->username[strlen(username)] = 0; 
	strncpy(myUser->password, password, strlen(password)); 
	myUser->password[strlen(password)] = 0; 
	myUser->count = 0; 
	myUser->messages = NULL; 
	myUser->next = NULL; 
	return myUser; 
}

void printUser(User *myUser)
{
	if(myUser){
		printf("This is the user %s\n", myUser->username); 
		for(int i = 0; i < myUser->count; i++){
			printf("%s\n", myUser->messages[i]->object); 
			printf("%s\n", myUser->messages[i]->text); 
		}
	}
}


void addUser(User **head, char *username, char *password)
{
	printf("Fatto\n"); 
	User *myUser = createUser(username, password); 
	while(*head){
		head = &(*head)->next; 
	}
	*head = myUser; 
	printf("È %s\n", (*head)->username); 
	fflush(stdout); 
}

void addMessage(User *myUser, char *object, char *text, char *idMessage){
	Message *myMessage = createMessage(object, text, idMessage); 
	myUser->messages = realloc(myUser->messages, sizeof(Message)*++myUser->count); 
	if(myUser->messages == NULL){
	}
	myUser->messages[myUser->count-1] = myMessage; 
}

void addMessageUser(User *head, char *username, char *object, char *text){
	
	char idMessage[SIZE_MESSAGE_ID+1]; 
	countMessages++; 
	snprintf(idMessage, sizeof(idMessage), "%06d", countMessages); 
	
	while(head){
		if(strcmp(head->username, username) == 0){
			break; 
		}
		head = head->next; 
	}
	if(head){
		addMessage(head, object, text,idMessage); 
	
	}
	
	wrMessage(username, object, text, idMessage, "back_messages.csv");


}

//questa elimina dato un indice, è la funzione di livello superiore che se la chiama le fornisce l'indice da eliminare
void delMessageByIndex(User *myUser, int index){
	Message *myMessage = myUser->messages[index]; 
	for(int i = index; i<myUser->count-1; i++){
		myUser->messages[i] = myUser->messages[i+1]; 
	}
	myUser->messages = realloc(myUser->messages, sizeof(Message)*--myUser->count); 
	free(myMessage); 
}

User *findUser(User *head, char *username){
	while(head){
		printf("Lo username %s\n", head->username); 
		fflush(stdout); 
		if(strcmp(head->username, username) == 0){
			break; 
		}
		head = head->next; 
	}
	if(head)return head; 
	return NULL; 

}

//questa funzione può fallire hai dato un oggetto non presente e quindi restituisce -1
int delMessageUser(User *head, char *username, char *object){
	User *myUser = findUser(head, username); 
	if(myUser == NULL) return -1; 
	for(int i = 0; i<myUser->count; i++){
		if(strcmp(myUser->messages[i]->object, object) == 0){
			delMessageByIndex(myUser, i); 
		}
	}
	return 0; 
}



void printUserMessage(User *head, char *username)
{
	User *myUser = findUser(head, username); 
	if(myUser){
		printf("I messaggi di %s sono :\n", username); 
		for(int i = 0; i<myUser->count; i++){
			printf("Object %s\n", myUser->messages[i]->object); 
			printf("Text %s\n", myUser->messages[i]->text); 
			printf("Id %s\n", myUser->messages[i]->idMessage);
		}
	
	}else{
		printf("L'utente non dispone di messagi\n"); 
	}
}

//this stuff is awful i know
void fillUsers(User **head, char *file){
	char buffUser[SIZE_USERNAME]; 
	char buffPass[SIZE_PASSWORD]; 
	char buff[1024]; 
	FILE *myFile = fopen(file, "r"); 
	if(myFile == NULL){
		fprintf(stderr, "Error opening file!"); 
		exit(EXIT_FAILURE); 
	
	}
	while(fgets(buff, 1024, myFile)){
		fill(buff,buffUser, buffPass);
		addUser(head, buffUser, buffPass); 
	} 
	fflush(stdout); 
} 
int checkUserPass(User *head, char *username, char *password){
	fflush(stdout); 
	while(head){
		fflush(stdout); 
		if( strcmp(head->username, username) == 0 && strcmp(head->password, password) == 0) return 0; 
		head = head ->next; 
	}
	return -1; 
}

void visualizeUsers(User *head){
	while(head){
		printf("Presente user %s\n", head->username); 
		printUser(head); 
		head = head->next; 
	}
}

void addMessageUserOld(User *head, char *username, char *object, char *text, char *idMessage){
	
	
	while(head){
		if(strcmp(head->username, username) == 0){
			break; 
		}
		head = head->next; 
	}
	if(head){
		addMessage(head, object, text,idMessage); 
	}


}

void fillMessagesUsers(User *head, char *file){
	char buffUser[SIZE_USERNAME+1]; 
	char buffObj[SIZE_OBJECT+1]; 
	char buffText[SIZE_TEXT+1]; 
	char buffIdMessage[SIZE_MESSAGE_ID+1]; 
	char buff[SIZE_BUFF]; 
	int v; 
 	FILE *myFile = fopen(file, "r"); 
 	if(myFile == NULL){
 		fprintf(stderr, "Error opening file\n"); 
 		exit(EXIT_FAILURE); 
 	
 	}
	while(fgets(buff, SIZE_BUFF, myFile)){
		fillMsg(buff, buffUser, buffObj, buffText, buffIdMessage, &v); 
		if(v!= 0){
			addMessageUserOld(head, buffUser, buffObj, buffText, buffIdMessage); 
			countMessages++; 
		}
	
	}

}



