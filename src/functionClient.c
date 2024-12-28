#include<unistd.h>

#include<errno.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include "../include/functionClient.h"
#include "../include/helper.h"
#include "../include/protocolUtilis.h"
#include "../include/utilityForFunctionClient.h"

//this function has to be only for the client part
int takeArgumentsClient(int argc, char **argv, char **serverAddress, short int *port){
	int i = 0; 
	char *endptr; 
	
	if(argc != 5)
	{
		//magari cambiare per avere un'usciata con EXIT_SUCCESS
		errFunction("Sintassi: eseguibile a indirizzo p porta"); 
	}
	
	while(i < argc -1)
	{
		if(strncmp(argv[i+1], "a", 1) == 0)
		{
			*serverAddress = argv[++i+1];
		}
		else if(strncmp(argv[i+1], "p",1) == 0)
		{
			*port = strtol(argv[++i+1], &endptr, 10); 
			if(*endptr)
			{
				errFunction("Porta non riconosciuta");
			}
		}
		i++; 	
	}
	return 0; 
}


// This function handles the client-side login process. It asks for a username and password, 
// sends the credentials to the server for authentication, and handles the response.


static void loginFunctionClient(int server_fd, char *username, char *password){
    int ret; 
    char c; 
    char authMessage[SIZE_AUTH_MESSAGE]; 

    while(1)
    {
        printf("Enter username\n"); 
        getValideUsername(username); 
        
        printf("Enter password\n"); 
        getValidePassword(password); 
        
        padBuff(username, strlen(username), SIZE_USERNAME); 
        padBuff(password, strlen(password), SIZE_PASSWORD); 
        
        buildMessage(authMessage, username, password,SIZE_AUTH_MESSAGE); 
       
        if(writeBuffSocket(server_fd, authMessage, SIZE_AUTH_MESSAGE) == -1)
        {
            errFunction("Write error on the server socket"); 
        }

        ret = readCom(server_fd, &c); 
        
        if(ret == 0){
            errFunction("Connection closed by the peer"); 
        } else if(ret == -1){
            errFunction("Read error from the server socket"); 
        }
        
        switch(c){
            case COMMAND_SUCCESS: 
                printf("Login successful\n"); 
                break; 
            case COMMAND_ERR_USER_NOT_FOUND: 
                printf("User not found in the system\n");
                break; 
            case COMMAND_ERR_NOT_MATCH_CREDENTIALS: 
                printf("Credentials mismatch, please enter correct credentials\n"); 
                break; 
            default: 
                if(writeCom(server_fd, COMMAND_CLOSE) == -1)
                {
                    errFunction("Write error on the server socket"); 
                }
                close(server_fd);
                exit(EXIT_SUCCESS); 
        }
        if(c == COMMAND_SUCCESS) break; 
    }
}

 
//This function handles the client-side registration process. It asks for a username and password, 
//sends them to the server for registration, and handles the response.

static void subFunctionClient(int server_fd, char *username, char *password)
{
    int ret; 
    char c; 
    char authMessage[SIZE_AUTH_MESSAGE]; 
    
    while(1)
    {
        printf("Enter a username with a length between 8 and 64 characters (inclusive)\n"); 
        getValideUsername(username); 
        
        printf("Enter a password with a length between 8 and 64 characters (inclusive)\n"); 
        printf("The password must contain at least one uppercase letter, one number, and one special character\n"); 
        getValidePassword(password); 
        
        padBuff(username, strlen(username), SIZE_USERNAME);
        padBuff(password, strlen(password), SIZE_PASSWORD);

        buildMessage(authMessage, username, password, SIZE_AUTH_MESSAGE); 
        
        if(writeBuffSocket(server_fd, authMessage, SIZE_AUTH_MESSAGE) == -1)
        {
            errFunction("Write error on the server socket"); 
        } 
        
        ret = readCom(server_fd, &c);
        
        if(ret == 0){
            errFunction("Connection closed by the server"); 
        } else if(ret == -1){
            errFunction("Read error from the server socket"); 
        }        
        
        switch(c){
            case COMMAND_SUCCESS: 
                printf("Registration successful\n"); 
                fflush(stdout); 
                break;
            case COMMAND_ERR_USER_ALREADY_EXISTS: 
                printf("User already exists in the system\n"); 
                fflush(stdout); 
                break; 
            default: 
                if(writeCom(server_fd, COMMAND_CLOSE) == -1)
                {
                    errFunction("Write error on the server socket"); 
                }
                close(server_fd);
                exit(EXIT_SUCCESS); 
        }
        if(c == COMMAND_SUCCESS) break; 
    }
}

// This function handles the authentication process, allowing the user to either log in or register.
// The user chooses whether they want to register (SUB) or log in (LOG).


static void authFunctionClient(char *username, char *password, int server_fd){
    char c; 
    char buffer[SIZE]; 
    
    printf("Enter SUB for registration or LOG for login\n"); 
    
    while(1){
        if(getInput(buffer, SIZE))
        {
            errFunction("Input reading failed"); 
        }
        if(strcmp(buffer, "SUB") == 0)
        {
            if(writeCom(server_fd, COMMAND_SUB) == -1)
            {
                errFunction("Write error on the server socket"); 
            }
            
            subFunctionClient(server_fd, username, password); 
            loginFunctionClient(server_fd, username, password); 
            break; 
            
        } else if(strcmp(buffer, "LOG") == 0)
        {
            if(writeCom(server_fd, COMMAND_LOG) == -1)
            {
                errFunction("Write error on the server socket"); 
            }
            
            loginFunctionClient(server_fd, username, password); 
            break; 
            
        } else {
            printf("Please enter SUB (for registration) or LOG (for login)...\n");
        }
    }
}

 
//This function handles the process of deleting a message from the server.
//It sends a delete message command and handles the response.

static void delMessageFunction(int server_fd)
{
    int ret; 
    char c; 
    char idMessage[SIZE_ID_MESSAGE + 1]; 
    
    if(writeCom(server_fd, COMMAND_DELETE_MSG) == -1)
    {
        errFunction("Write error on the server socket"); 
    }

    printf("Enter ID of the message to delete\n"); 
    if(getInput(idMessage, SIZE_ID_MESSAGE + 1) == -1)
    {
        errFunction("Error reading message ID to delete"); 
    }
    while(getchar() != '\n'); 

    if(writeBuffSocket(server_fd, idMessage, SIZE_ID_MESSAGE) == -1)
    {
        errFunction("Write error on the server socket"); 
    }

    ret = readCom(server_fd, &c); 

    if(ret == 0){
        errFunction("Connection closed by the peer"); 
    } else if(ret == -1){
        errFunction("Read error from the server socket"); 
    }    
    
    switch(c){
        case COMMAND_FAILURE: 
            printf("Message deletion failed");
            break; 
        case COMMAND_SUCCESS: 
            printf("Message deleted successfully");
            break; 
        default:  
            if(writeCom(server_fd, COMMAND_CLOSE) == -1)
            {
                errFunction("Write error on the server socket"); 
            }
            close(server_fd);
            exit(EXIT_SUCCESS); 
    }
}

// This function handles posting a new message to the server. It collects the object and text of the message, 
// sends the message to the server, and handles the response.

static void postMessageFunction(int server_fd)
{
    int ret; 
    char c; 
    char objBuffer[SIZE_OBJECT + 1]; 
    char textBuffer[SIZE_TEXT + 1]; 
    
    char msgMessage[SIZE_MSG_MESSAGE];
    
    if(writeCom(server_fd, COMMAND_POST_MSG) == -1)
    {
        errFunction("Write error on the server socket"); 
    }
    
    printf("Enter the object of the message (maximum 64 characters):\n"); 
    if(getInput(objBuffer, SIZE_OBJECT + 1)){
        errFunction("Error reading message object from stdin"); 
    }

    printf("Enter the text of the message (maximum 160 characters):\n"); 
    fflush(stdout);
    if(getInput(textBuffer, SIZE_TEXT + 1)){
        errFunction("Error reading message text from stdin\n"); 
    }

    padBuff(objBuffer, strlen(objBuffer), SIZE_OBJECT); 
    padBuff(textBuffer, strlen(textBuffer), SIZE_TEXT); 
    buildMessage(msgMessage, objBuffer, textBuffer, SIZE_MSG_MESSAGE); 

    if((ret = writeBuffSocket(server_fd, msgMessage, SIZE_MSG_MESSAGE)) == -1)
    {
        errFunction("Write error on the server socket"); 
    }

    ret = readCom(server_fd, &c); 

    if(ret == 0){
        errFunction("Connection closed by the peer"); 
    } else if(ret == -1){
        errFunction("Read error from the server socket"); 
    }    

    switch(c){
        case COMMAND_SUCCESS: 
            printf("Message successfully posted!!"); 
            fflush(stdout); 
            break; 
        case COMMAND_FAILURE: 
            printf("Unable to post message"); 
            fflush(stdout);
            break; 
        default:
            if(writeCom(server_fd, COMMAND_CLOSE) == -1)
            {
                errFunction("Write error on the server socket"); 
            }
            close(server_fd); 
            exit(EXIT_SUCCESS); 
    }
}




// Function to print a generic message in the message board format.
// Extracts specific fields from the message string and displays them.

static void printGenericMessage(char *message)
{
    char objBuffer[SIZE_OBJECT + 1];
    char textBuffer[SIZE_TEXT + 1];
    char idBuffer[SIZE_ID_MESSAGE + 1];
    char authorBuffer[SIZE_USERNAME + 1];

    extractField(message, objBuffer, 0, SIZE_OBJECT);
    extractField(message, textBuffer, SIZE_OBJECT, SIZE_OBJECT + SIZE_TEXT);
    extractField(message, idBuffer, SIZE_OBJECT + SIZE_TEXT, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE);
    extractField(message, authorBuffer, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE + SIZE_USERNAME);

    printf("\n");
    printf("<-----Message----->\n");
    printf("Subject: %s\n", objBuffer);
    printf("Text: %s\n", textBuffer);
    printf("Message ID: %s\n", idBuffer);
    printf("Author: %s\n", authorBuffer);
    printf("<------------------->\n");
}

//Function to print a personal message in the message board format.
//Extracts specific fields from the message string and displays them.

static void printPersonalMessage(char *message)
{
    char objBuffer[SIZE_OBJECT + 1];
    char textBuffer[SIZE_TEXT + 1];
    char idBuffer[SIZE_ID_MESSAGE + 1];

    extractField(message, objBuffer, 0, SIZE_OBJECT);
    extractField(message, textBuffer, SIZE_OBJECT, SIZE_OBJECT + SIZE_TEXT);
    extractField(message, idBuffer, SIZE_OBJECT + SIZE_TEXT, SIZE_OBJECT + SIZE_TEXT + SIZE_ID_MESSAGE);

    printf("\n");
    printf("<-----Message----->\n");
    printf("Subject: %s\n", objBuffer);
    printf("Text: %s\n", textBuffer);
    printf("Message ID: %s\n", idBuffer);
    printf("<------------------->\n");
}

// Function to view all personal messages.
// Sends a command to the server and retrieves messages to display.

static void viewMessageFunction(int server_fd)
{
    int ret, n;
    char c;
    char msgMessage[SIZE_PERSONAL_COMPLETE_MESSAGE];

    if (writeCom(server_fd, COMMAND_VIEW_MSG) == -1)
    {
        errFunction("Error writing to server_fd");
    }

    n = numberMessages(server_fd);
    if (n == -1)
    {
        errFunction("Error converting the number of messages on the board");
    }

    for (int i = 0; i < n; i++)
    {
        if (readBuffSocket(server_fd, msgMessage, SIZE_PERSONAL_COMPLETE_MESSAGE) == -1)
        {
            errFunction("Error reading a message from the board");
        }
        printPersonalMessage(msgMessage);
    }

    ret = readCom(server_fd, &c);

    if (ret == 0)
    {
        errFunction("Connection closed by peer");
    }
    else if (ret == -1)
    {
        errFunction("Error reading from server_fd");
    }

    switch (c)
    {
        case COMMAND_SUCCESS:
            break;
        default:
            if (writeCom(server_fd, COMMAND_CLOSE) == -1)
            {
                errFunction("Error writing to server_fd");
            }
            close(server_fd);
            exit(EXIT_SUCCESS);
    }
}

// Function to view all messages on the board.
// Sends a command to the server and retrieves all messages to display.

static void viewAllMessageFunction(int server_fd)
{
    int ret;
    char c;
    char msgMessage[SIZE_GENERIC_COMPLETE_MESSAGE];
    int n;

    if (writeCom(server_fd, COMMAND_VIEW_ALL_MSG) == -1)
    {
        errFunction("Error writing to server_fd");
    }

    n = numberMessages(server_fd);

    for (int i = 0; i < n; i++)
    {
        if (readBuffSocket(server_fd, msgMessage, SIZE_GENERIC_COMPLETE_MESSAGE) == -1)
        {
            errFunction("Error reading a message from the board");
        }
        printGenericMessage(msgMessage);
    }

    if (readCom(server_fd, &c) == -1)
    {
        errFunction("Error writing to server_fd");
    }

    if (ret == 0)
    {
        errFunction("Connection closed by peer");
    }
    else if (ret == -1)
    {
        errFunction("Error reading from server_fd");
    }

    switch (c)
    {
        case COMMAND_SUCCESS:
            break;
        default:
            if (writeCom(server_fd, COMMAND_CLOSE) == -1)
            {
                errFunction("Error writing to server_fd");
            }
            close(server_fd);
            exit(EXIT_SUCCESS);
    }
}

// Client function to interact with the message board server.
// Handles authentication and presents the user with menu options.

void clientFunc(int server_fd)
{
    int ret;
    char c;
    char username[SIZE_USERNAME + 1];
    char password[SIZE_PASSWORD + 1];

    ret = readCom(server_fd, &c);

    if (ret == 0)
    {
        errFunction("Connection closed by the server");
    }
    else if (ret == -1)
    {
        errFunction("Error reading from server_fd");
    }

    if (c == COMMAND_AUTH)
    {
        authFunctionClient(username, password, server_fd);
    }
    else
    {
        printf("Used code is %d\n", COMMAND_AUTH);
        errFunction("Communication error");
    }

    while (1)
    {
        printf("\nMenu\n");
        printf("1: Insert a new message\n");
        printf("2: View messages\n");
        printf("3: View all messages\n");
        printf("4: Delete a message\n");
        printf("5: Quit\n");
        c = getchar();
        while (getchar() != '\n');

        switch (c)
        {
            case '1':
                postMessageFunction(server_fd);
                break;
            case '2':
                viewMessageFunction(server_fd);
                break;
            case '3':
                viewAllMessageFunction(server_fd);
                break;
            case '4':
                delMessageFunction(server_fd);
                break;
            case '5':
                if (writeCom(server_fd, COMMAND_CLOSE) == -1)
                {
                    errFunction("Error writing to server_fd");
                }
                exit(EXIT_SUCCESS);
            default:
                printf("Unrecognized menu option");
        }
    }
}


