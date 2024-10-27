#ifndef MESSAGE_LIB_H
#define MESSAGE_LIB_H

#define SIZE_OBJECT 64
#define SIZE_TEXT 160
#define SIZE_MESSAGE_ID 2

#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64




typedef struct message {
    char object[SIZE_OBJECT]; 
    char text[SIZE_TEXT]; 
    char idMessage[SIZE_MESSAGE_ID];
    int value; 
} Message; 

typedef struct user {
    char username[SIZE_USERNAME]; 
    char password[SIZE_PASSWORD]; 
    int count; 
    Message **messages; 
    struct user *next; 
} User; 

Message *createMessage(char *object, char *text); 
void printMessage(Message *myMessage); 
User *createUser(char *username, char *password); 
void printUser(User *myUser); 
void addUser(User **head, char *username, char *password); 
void addMessage(User *myUser, char *object, char *text); 
void addMessageUser(User *head, char *username, char *object, char *text); 
void delMessageByIndex(User *myUser, int index); 
User *findUser(User *head, char *username); 
int delMessageUser(User *head, char *username, char *object); 
void printUserMessage(User *head, char *username); 
int checkUserPass(User *head, char *password, char *username); 
void fillUsers(User **head, char *file); 
void visualizeUsers(User *head); 
void fillMessagesUsers(User *head, char *file); 

#endif // MESSAGE_LIB_H

