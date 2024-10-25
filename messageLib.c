#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"messageLib.h"
#include"helper.h"

//idMessage is not implemented
Message *createMessage(char *object, char *text){
	Message *myMessage = malloc(sizeof(Message)); 
	if(myMessage == NULL){
		fprintf(stderr, "Error in malloc occured!"); 
		exit(EXIT_FAILURE); 
	}
	strncpy(myMessage->object, object, strlen(object)); 
	myMessage->object[strlen(object)] = 0; 
	strncpy(myMessage->text, text, strlen(text)); 
	myMessage->text[strlen(text)] = 0; 
	myMessage->value = 1; 
	return myMessage; 
}


void printMessage(Message *myMessage){
	if(myMessage){
		printf("Object is %s text is %s\n", myMessage->object, myMessage->text); 
	}
}



User *createUser(char *username, char *password){
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

void printUser(User *myUser){
	if(myUser){
		printf("This is the user %s\n", myUser->username); 
		for(int i = 0; i < myUser->count; i++){
			printf("%s\n", myUser->messages[i]->object); 
		}
	}
}


void addUser(User **head, char *username, char *password){
	User *myUser = createUser(username, password); 
	while(*head){
		head = &(*head)->next; 
	}
	*head = myUser; 
}

void addMessage(User *myUser, char *object, char *text){
	Message *myMessage = createMessage(object, text); 
	myUser->messages = realloc(myUser->messages, sizeof(Message)*++myUser->count); 
	if(myUser->messages == NULL){
		fprintf(stderr, "Error in realloc occured!\n"); 
		exit(EXIT_FAILURE); 
	}
	myUser->messages[myUser->count-1] = myMessage; 
}

void addMessageUser(User *head, char *username, char *object, char *text){
	while(head){
		if(strcmp(head->username, username) == 0){
			break; 
		}
		head = head->next; 
	}
	if(head){
		addMessage(head, object, text); 
	
	}


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



void printUserMessage(User *head, char *username){
	User *myUser = findUser(head, username); 
	if(myUser){
		printf("I messaggi di %s sono :\n", username); 
		for(int i = 0; i<myUser->count; i++){
			printf("Object %s\n", myUser->messages[i]->object); 
			printf("Text %s\n", myUser->messages[i]->text); 
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
		head = head->next; 
	}
}

void fillMessagesUsers(User *head, char *file){
	char buffUser[1024]; 
	char buffObj[1024]; 
	char buffText[1024]; 
	char buff[1024]; 
	int v; 
 	FILE *myFile = fopen(file, "r"); 
 	if(myFile == NULL){
 		fprintf(stderr, "Error opening file\n"); 
 		exit(EXIT_FAILURE); 
 	
 	}
	while(fgets(buff, 1024, myFile)){
		fillMsg(buff, buffUser, buffObj, buffText, &v); 
		if(v!= 0){
			addMessageUser(head, buffUser, buffObj, buffText); 
		}
	
	}

}



