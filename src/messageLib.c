#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "../include/messageLib.h"
#include "../include/helper.h"

#define SIZE 1024

// Function to create a new message
// This function creates a new message and initializes its fields (object, text, and id).
// It returns a pointer to the newly created Message object.
Message *createMessage(char *object, char *text, char *id)
{
    Message *myMessage = malloc(sizeof(Message)); 
    if(myMessage == NULL){
        errFunction("Error in malloc occured!");
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

// Function to print a message
// This function prints the object and text of a message.
void printMessage(Message *myMessage)
{
    if(myMessage){
        printf("Object is %s text is %s\n", myMessage->object, myMessage->text); 
    }
}

// Function to create a new user
// This function creates a new user and initializes its fields (username, password, count, messages, next).
// It returns a pointer to the newly created User object.
User *createUser(char *username, char *password)
{
    User *myUser = malloc(sizeof(User)); 
    if(myUser == NULL){
        errFunction("Error in malloc occured!");
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

// Function to print user information
// This function prints the username and all messages of the user.
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

// Function to add a new user to the bulletin board
// This function creates a new user and adds it to the list of users on the bulletin board.
void addUser(BulletinBoard *myBoard, char *username, char *password)
{
    User *newUser = createUser(username, password); 
    User *current;  

    if( myBoard->head == NULL)
    {
        myBoard->head = newUser; 
        return; 
    }

    current = myBoard->head; 

    while(current->next)
    {
        current = current->next; 
    }

    current->next = newUser; 
}

// Function to add a message to a user
// This function creates a new message and adds it to the user's messages list.
void addMessage(User *myUser, char *object, char *text, char *idMessage){
    Message *myMessage = createMessage(object, text, idMessage); 
    myUser->messages = realloc(myUser->messages, sizeof(Message)*++myUser->count); 
    if(myUser->messages == NULL){
        errFunction("Error in realloc occured");
    }
    myUser->messages[myUser->count-1] = myMessage; 
}

// Function to add a message to a user's messages list on the board
// This function adds a message to a specific user identified by the username on the bulletin board.
void addMessageUser(BulletinBoard *myBoard, char *username, char *object, char *text, char *idMessage){
    User *current = myBoard->head; 
    
    ++myBoard->msgCount; 
    
    while(current){
        if(strcmp(current->username, username) == 0){
            break; 
        }
        current = current->next; 
    }
    if(current){
        addMessage(current, object, text, idMessage); 
    }
}

// Function to delete a message by index
// This function deletes a message from the user's messages list given the index of the message.
void delMessageByIndex(User *myUser, int index){
    Message *myMessage = myUser->messages[index]; 
    
    for(int i = index; i < myUser->count-1; i++)
    {
        myUser->messages[i] = myUser->messages[i+1]; 
    }
    myUser->messages = realloc(myUser->messages, sizeof(Message)*--myUser->count); 
    free(myMessage); 
}

// Function to find a user by username
// This function searches for a user by username in the bulletin board.
// It returns a pointer to the User object if found, or NULL if the user doesn't exist.
User *findUser(BulletinBoard *myBoard, char *username){
    User *current = myBoard->head; 
    
    while(current)
    { 
        if(strcmp(current->username, username) == 0){
            break; 
        }
        current = current->next; 
    }
    if(current)return current; 
    return NULL; 
}

// Function to delete a message from a user's messages by message ID
// This function deletes a message from a user's messages list using the message ID.
// It returns 0 if the message is deleted successfully, or -1 if the message is not found.
int delMessageUser(BulletinBoard *myBoard, char *username, char *idMessage){
    User *myUser = findUser(myBoard, username); 
    if(myUser == NULL) return -1; 
    for(int i = 0; i < myUser->count; i++)
    {
        if(strcmp(myUser->messages[i]->idMessage, idMessage) == 0)
        {
            delMessageByIndex(myUser, i); 
            --myBoard->msgCount;
            return 0; 
        }
    }
    return -1; 
}

// Function to print all messages of a specific user
// This function prints all the messages of a user on the bulletin board.
void printUserMessage(BulletinBoard *myBoard, char *username)
{
    User *myUser = findUser(myBoard, username); 
    if(myUser){
        printf("The messages of %s are:\n", username); 
        for(int i = 0; i < myUser->count; i++){
            printf("Object %s\n", myUser->messages[i]->object); 
            printf("Text %s\n", myUser->messages[i]->text); 
            printf("Id %s\n", myUser->messages[i]->idMessage);
        }
    } else {
        printf("User has no messages\n"); 
    }
}

// Function to check if the username and password match
// This function checks if the provided username and password match any existing user on the board.
// It returns 0 if the username and password match, or -1 if they don't.
int checkUserPass(BulletinBoard *myBoard, char *username, char *password)
{
    User *current = myBoard->head;
    
    while(current)
    {
        if( strcmp(current->username, username) == 0 && strcmp(current->password, password) == 0) return 0; 
        current = current->next; 
    }
    return -1; 
}

// Function to visualize all users in the bulletin board
// This function prints all the users and their associated messages from the bulletin board.
void visualizeUsers(BulletinBoard *myBoard)
{
    User *current = myBoard->head; 
    while(current)
    {
        printf("User %s is present\n", current->username); 
        printUser(current); 
        current = current->next; 
    }
}

// Function to create a new bulletin board
// This function initializes a new bulletin board with default values.
// It returns a pointer to the newly created BulletinBoard object.
BulletinBoard *createBulletinBoard()
{
    BulletinBoard *myBoard = malloc(sizeof(BulletinBoard)); 
    if(myBoard == NULL)
    {
        errFunction("Error in malloc occured!");
    }
    myBoard->head = NULL; 
    myBoard->msgCount = 0; 
    myBoard->idCount = 0; 
    return myBoard; 
}


