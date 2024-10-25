#ifndef MESSAGE_LIB_H
#define MESSAGE_LIB_H

#define MAX_OBJECT 64
#define MAX_TEXT 160

#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64

#define MAX_IDMESSAGE 24



typedef struct message {
    char object[MAX_OBJECT]; 
    char text[MAX_TEXT]; 
    char idMessage[MAX_IDMESSAGE];
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

