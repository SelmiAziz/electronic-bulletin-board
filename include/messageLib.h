#ifndef MESSAGE_LIB_H
#define MESSAGE_LIB_H

#include"commServerClientConfig.h"


typedef struct message{
    char object[SIZE_OBJECT]; 
    char text[SIZE_TEXT]; 
    char idMessage[SIZE_ID_MESSAGE];
    int value; 
} Message; 

typedef struct user{
    char username[SIZE_USERNAME]; 
    char password[SIZE_PASSWORD]; 
    int count; 
    Message **messages; 
    struct user *next; 
} User; 

typedef struct bulletinBoard{
	User *head;
	int msgCount; 
	int idCount; 
}BulletinBoard; 


Message *createMessage(char *object, char *text, char *id); 
BulletinBoard *createBulletinBoard(); 
User *createUser(char *username, char *password);
 
void printMessage(Message *myMessage); 
void printUser(User *myUser); 
void printUserMessage(BulletinBoard *myBoard, char *username);
void visualizeUsers(BulletinBoard *myBoard); 

void addUser(BulletinBoard *myBoard, char *username, char *password); 
void addMessage(User *myUser, char *object, char *text, char *id); 
void addMessageUser(BulletinBoard *myBoard, char *username, char *object, char *text, char *idMessage); 

void delMessageByIndex(User *myUser, int index);
int delMessageUser(BulletinBoard *myBoard, char *username, char *object); 

User *findUser(BulletinBoard *myBoard, char *username);  
int checkUserPass(BulletinBoard *myBoard, char *password, char *username);

#endif // MESSAGE_LIB_H

