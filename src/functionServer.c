#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

#include<pthread.h>
#include <unistd.h>   

#include "../include/functionServer.h"
#include "../include/helper.h"
#include "../include/fileMessageLib.h"
#include "../include/protocolUtilis.h"
#include "../include/utilityForFunctionServer.h"


char *fileMessages = MESSAGES_FILE;


//la funzione ritorna -1 errno invariato errore di lettura
//la funzione ritorna 0 canale chiuso
//la funzione ritorna -1 errno uguale a EWOULDBLOCK oppure EAGAIN errore di timeout
//deve essere sviluppata una funzione read per il client che non abbia Timeout
static int readTimeout(int fd, void *buffer, size_t total_bytes)
{
    size_t bytes_read = 0;
    ssize_t result;
    int attempts = 0;

    while (bytes_read < total_bytes) {
        result = read(fd, (char *)buffer + bytes_read, total_bytes - bytes_read);

        if (result < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                attempts++;
                if (attempts == MAX_READ_ATTEMPTS) {
                    return -1;
                }
                continue; 
            }
            return -1; 
        } else if (result == 0) {
            return 0; 
        }

        bytes_read += result;
    }

    return bytes_read; 
}


// The following function allows the client to post and insert a message into the bulletin board.

static int postMessageFunction(int socket, BulletinBoard *myBoard, char *username)
{
    int ret;
    char c;
    char objBuffer[SIZE_OBJECT + 1];
    char textBuffer[SIZE_TEXT + 1];
    char msgMessage[SIZE_MSG_MESSAGE];
    char idMsgBuffer[SIZE_ID_MESSAGE + 1];

    ret = readTimeout(socket, msgMessage, SIZE_MSG_MESSAGE);

    if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
    {
        writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
        close(socket);
        pthread_exit(NULL); 
    }
    if (ret == -1) 
    {
        if (writeCom(socket, COMMAND_CLOSE) == -1)
        {
            errFunction("Error writing to socket"); 
        }
        close(socket);
        pthread_exit(NULL); 
    }

    extractField(msgMessage, objBuffer, 0, SIZE_OBJECT); 
    extractField(msgMessage, textBuffer, SIZE_OBJECT, SIZE_OBJECT + SIZE_TEXT); 

    snprintf(idMsgBuffer, sizeof(idMsgBuffer), "%06d", ++myBoard->idCount); 
    addMessageUser(myBoard, username, objBuffer, textBuffer, idMsgBuffer);
    writeMessageFile(username, objBuffer, textBuffer, idMsgBuffer, fileMessages); 

    printUserMessage(myBoard, username);  

    if (writeCom(socket, COMMAND_SUCCESS) == -1)
    {
        errFunction("Error writing to socket"); 
    }
}





// The following functions allow viewing messages on the bulletin board. 
// Specifically, viewAllMessageFunction allows viewing all messages, including those authored by others.
// viewMessageFunction allows viewing only messages authored by the user.

static void viewAllMessageFunction(int socket, BulletinBoard *myBoard)
{
    int ret;
    char c;
    char numMsgBuff[SIZE_NUM_MSG];
    char msgMessage[SIZE_GENERIC_COMPLETE_MESSAGE];
    char objBuffer[SIZE_OBJECT + 1];
    char textBuffer[SIZE_TEXT + 1];
    char idMessageBuffer[SIZE_ID_MESSAGE + 1];
    char authorBuffer[SIZE_USERNAME + 1];
    User *currentUser;

    sprintf(numMsgBuff, "%d", myBoard->msgCount);
    padBuff(numMsgBuff, strlen(numMsgBuff), SIZE_NUM_MSG);

    if (writeBuffSocket(socket, numMsgBuff, SIZE_NUM_MSG) == -1)
    {
        errFunction("Error writing to socket");
    }

    currentUser = myBoard->head;
    while (currentUser)
    {
        for (int i = 0; i < currentUser->count; i++)
        {
            copyInBuffer(objBuffer, currentUser->messages[i]->object);
            copyInBuffer(textBuffer, currentUser->messages[i]->text);
            copyInBuffer(idMessageBuffer, currentUser->messages[i]->idMessage);
            copyInBuffer(authorBuffer, currentUser->username);

            padBuff(objBuffer, strlen(objBuffer), SIZE_OBJECT);
            padBuff(textBuffer, strlen(textBuffer), SIZE_TEXT);
            padBuff(idMessageBuffer, strlen(idMessageBuffer), SIZE_ID_MESSAGE);
            padBuff(authorBuffer, strlen(authorBuffer), SIZE_USERNAME);

            buildGenericUserMessage(msgMessage, objBuffer, textBuffer, idMessageBuffer, authorBuffer);

            if (writeBuffSocket(socket, msgMessage, SIZE_GENERIC_COMPLETE_MESSAGE) == -1)
            {
                errFunction("Error writing to socket");
            }
        }
        currentUser = currentUser->next;
    }

    if (writeCom(socket, COMMAND_SUCCESS) == -1)
    {
        errFunction("Error writing to socket");
    }
}



static void viewMessageFunction(int socket, BulletinBoard *myBoard, char *username)
{
    int ret;
    char c;
    char numMsgBuff[SIZE_NUM_MSG];
    char msgMessage[SIZE_PERSONAL_COMPLETE_MESSAGE];
    char objBuffer[SIZE_OBJECT + 1];
    char textBuffer[SIZE_TEXT + 1];
    char idMessageBuffer[SIZE_ID_MESSAGE + 1];

    User *currentUser = findUser(myBoard, username);
    if (currentUser == NULL)
    {
        // Uncomment this section if required for specific handling when the user is not found.
        /*
        if (writeCom(socket, COMMAND_CLOSE) == -1)
        {
            errFunction("Error writing to socket");
        }
        */
    }
    else
    {
        // Uncomment this section if needed for specific handling when the user is found.
        /*
        if (writeCom(socket, COMMAND_SUCCESS) == -1)
        {
            errFunction("Error writing to socket");
        }
        */
    }

    sprintf(numMsgBuff, "%d", currentUser->count);
    padBuff(numMsgBuff, strlen(numMsgBuff), SIZE_NUM_MSG);

    if (writeBuffSocket(socket, numMsgBuff, SIZE_NUM_MSG) == -1)
    {
        errFunction("Error writing to socket");
    }

    for (int i = 0; i < currentUser->count; i++)
    {
        copyInBuffer(objBuffer, currentUser->messages[i]->object);
        copyInBuffer(textBuffer, currentUser->messages[i]->text);
        copyInBuffer(idMessageBuffer, currentUser->messages[i]->idMessage);

        padBuff(objBuffer, strlen(objBuffer), SIZE_OBJECT);
        padBuff(textBuffer, strlen(textBuffer), SIZE_TEXT);
        padBuff(idMessageBuffer, strlen(idMessageBuffer), SIZE_ID_MESSAGE);

        buildPersonalUserMessage(msgMessage, objBuffer, textBuffer, idMessageBuffer);

        if (writeBuffSocket(socket, msgMessage, SIZE_PERSONAL_COMPLETE_MESSAGE) == -1)
        {
            errFunction("Error writing to socket");
        }
    }

    if (writeCom(socket, COMMAND_SUCCESS) == -1)
    {
        errFunction("Error writing to socket");
    }
}





// The following function allows the deletion of a message from the bulletin board based on an ID sent by the client.


static void delMessageFunction(int socket, BulletinBoard *myBoard, char *username)
{
    int ret;
    char c;
    char idMessage[SIZE_ID_MESSAGE + 1];
    User *user;

    ret = readTimeout(socket, idMessage, SIZE_ID_MESSAGE);

    if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
    {
        writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
        close(socket);
        pthread_exit(NULL);
    }
    if (ret == -1) 
    {
        if (writeCom(socket, COMMAND_CLOSE) == -1) 
        {
            errFunction("Error writing to socket");
        }
        close(socket);
        pthread_exit(NULL);
    }

    idMessage[SIZE_ID_MESSAGE] = 0;

    if (myBoard->idCount < strtol(idMessage, NULL, 10)) 
    { 
        if (writeCom(socket, COMMAND_FAILURE) == -1) 
        {
            errFunction("Error writing to socket");
        }
    } 
    else 
    {
        if (delMessageUser(myBoard, username, idMessage) == -1) 
        {
            if (writeCom(socket, COMMAND_FAILURE) == -1) 
            {
                errFunction("Error writing to socket");
            }
        } 
        else 
        {
            delMessageFile(fileMessages, idMessage);
            if (writeCom(socket, COMMAND_SUCCESS) == -1) 
            {
                errFunction("Error writing to socket");
            }
        }
    }
}





// The authFuncServer function allows the client to authenticate: register a new account or log in to an existing one.
// It makes use of the auxiliary functions subFunctionServer and logFunctionServer.

static void subFunctionServer(int socket, BulletinBoard *myBoard, char *username, char *password)
{
    int ret;
    char authMessage[SIZE_AUTH_MESSAGE];

    while (1) 
    {
        ret = readTimeout(socket, authMessage, SIZE_AUTH_MESSAGE);

        if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
        {
            if (writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS) == -1) 
            {
                errFunction("Error writing to socket");
            }
            close(socket);
            pthread_exit(NULL);
        }
        if (ret == -1) 
        {
            if (writeCom(socket, COMMAND_CLOSE) == -1) 
            {
                errFunction("Error writing to socket");
            }
            close(socket);
            pthread_exit(NULL);
        }

        extractField(authMessage, username, 0, SIZE_USERNAME);
        extractField(authMessage, password, SIZE_USERNAME, SIZE_USERNAME + SIZE_PASSWORD);

        if (findUser(myBoard, username)) 
        {
            if (writeCom(socket, COMMAND_ERR_USER_ALREADY_EXISTS) == -1) 
            {
                errFunction("Error writing to socket");
            }
        } 
        else 
        {
            if (writeCom(socket, COMMAND_SUCCESS) == -1) 
            {
                errFunction("Error writing to socket");
            }
            break;
        }
    }

    addUser(myBoard, username, password);
    writeUserFile(username, password, USERS_FILE);
}



static void logFunctionServer(int socket, BulletinBoard *myBoard, char *username, char *password)
{
    char authMessage[SIZE_AUTH_MESSAGE];
    int ret;

    while (1) 
    {
        ret = readTimeout(socket, authMessage, SIZE_AUTH_MESSAGE);

        if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
        {
            if (writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS) == -1) 
            {
                errFunction("Error writing to socket");
            }
            close(socket);
            pthread_exit(NULL);
        }
        if (ret == -1) 
        {
            if (writeCom(socket, COMMAND_CLOSE) == -1) 
            {
                errFunction("Error writing to socket");
            }
            close(socket);
            pthread_exit(NULL);
        }

        extractField(authMessage, username, 0, SIZE_USERNAME);
        extractField(authMessage, password, SIZE_USERNAME, SIZE_USERNAME + SIZE_PASSWORD);

        if (findUser(myBoard, username) == NULL) 
        {
            if (writeCom(socket, COMMAND_ERR_USER_NOT_FOUND) == -1) 
            {
                errFunction("Error writing to socket");
            }
            continue;
        }
        if (checkUserPass(myBoard, username, password)) 
        {
            if (writeCom(socket, COMMAND_ERR_NOT_MATCH_CREDENTIALS) == -1) 
            {
                errFunction("Error writing to socket");
            }
        } 
        else 
        {
            if (writeCom(socket, COMMAND_SUCCESS) == -1) 
            {
                errFunction("Error writing to socket");
            }
            break;
        }
    }
}



static void authFuncServer(int client_fd, BulletinBoard *myBoard, char *username, char *password)
{
    char c;
    int ret;

    if (writeCom(client_fd, COMMAND_AUTH) == -1) 
    {
        errFunction("Error writing to socket");
    }

    ret = readCom(client_fd, &c);
    if (ret == -1) 
    {
        errFunction("Error reading from socket");
    }

    if (c == COMMAND_SUB) 
    {
        subFunctionServer(client_fd, myBoard, username, password);
        logFunctionServer(client_fd, myBoard, username, password);
    } 
    else if (c == COMMAND_LOG) 
    {
        logFunctionServer(client_fd, myBoard, username, password);
    } 
    else 
    {
        if (writeCom(client_fd, COMMAND_CLOSE) == -1) 
        {
            errFunction("Error writing to socket");
        }
    }
}




void *worker(void *arg) {    
    ThreadData *tData = (ThreadData*) arg;    

    char username[SIZE_USERNAME+1];    
    char password[SIZE_PASSWORD+1];    
    char c;    
    int ret;    
    int client_fd = tData->socket;    
    BulletinBoard *myBoard = tData->myBoard;        

    free(tData);    
    tData = NULL;        

    authFuncServer(client_fd, myBoard, username, password);       

    while(1){        
        ret = readCom(client_fd, &c);        
        if(ret == 0)        
        {            
            pthread_exit(NULL);  	        
        } else if( ret == -1)        
        {            
            errFunction("Error reading from socket");         
        }                         

        switch(c){            
            case COMMAND_QUIT:                 
                pthread_exit(NULL);             
            break;                       

            case COMMAND_VIEW_MSG:                 
                viewMessageFunction(client_fd, myBoard, username);	            
            break;                           

            case COMMAND_VIEW_ALL_MSG:                 
                viewAllMessageFunction(client_fd, myBoard);             
            break;              

            case COMMAND_POST_MSG:                 
                postMessageFunction(client_fd, myBoard, username);             
            break;              

            case COMMAND_DELETE_MSG:                 
                delMessageFunction(client_fd, myBoard, username);             
            break;              

            default:                 
                if(writeCom(client_fd, COMMAND_CLOSE) == -1)                
                {                     
                    errFunction("Error writing to socket");                 
                }                 
                close(client_fd);                 
                pthread_exit(NULL);         
        }	     
    } 
}

