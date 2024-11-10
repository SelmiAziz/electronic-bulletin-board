#ifndef MESSAGE_LIB_H
#define MESSAGE_LIB_H

#define SIZE_OBJECT 64
#define SIZE_TEXT 160
#define SIZE_MESSAGE_ID 6

#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64




typedef struct message{
    char object[SIZE_OBJECT]; 
    char text[SIZE_TEXT]; 
    char idMessage[SIZE_MESSAGE_ID];
    int value; 
} Message; 



typedef struct user{
    char username[SIZE_USERNAME]; 
    char password[SIZE_PASSWORD]; 
    int count; 
    Message **messages; 
    struct user *next; 
} User; 

//Da implementare il bulletinBoard
typedef struct bulletinBoard{
	User *head;
	int msgCount; 
}BulletinBoard; 


Message *createMessage(char *object, char *text, char *id); 
BulletinBoard *createBulletinBoard(); 
User *createUser(char *username, char *password); 
void printMessage(Message *myMessage); 
void printUser(User *myUser); 
void addUser(BulletinBoard *myBoard, char *username, char *password); 
void addMessage(User *myUser, char *object, char *text, char *id); 
void addMessageUser(BulletinBoard *myBoard, char *username, char *object, char *text, char *file); 
void delMessageByIndex(User *myUser, int index); 
User *findUser(BulletinBoard *myBoard, char *username); 
int delMessageUser(BulletinBoard *myBoard, char *username, char *object);  
void printUserMessage(BulletinBoard *myBoard, char *username); 
int checkUserPass(BulletinBoard *myBoard, char *password, char *username);
void fillUsers(BulletinBoard *myBoard, char *file); 
void visualizeUsers(BulletinBoard *myBoard); 
void fillMessagesUsers(BulletinBoard *myBoard, char *file); 

#endif // MESSAGE_LIB_H

