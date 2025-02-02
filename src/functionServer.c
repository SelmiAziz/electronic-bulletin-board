#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

#include<pthread.h>
#include <unistd.h>   

#include "../include/functionServer.h"
#include "../include/helper.h"
#include "../include/fileMessageLib.h"
#include "../include/messageLib.h"
#include "../include/protocolUtilis.h"
#include "../include/utilityForFunctionServer.h"
#include "../include/mutexLib.h"
#include "../include/serverConfig.h"

char *fileMessages = MESSAGES_FILE;


// The following function allows the client to post and insert a message into the bulletin board.

static int postMessageFunction(int client_fd, BulletinBoard *myBoard, char *username)
{
    int ret;
    char c;
    char objBuffer[SIZE_OBJECT + 1];
    char textBuffer[SIZE_TEXT + 1];
    char msgMessage[SIZE_MSG_MESSAGE];
    char idMsgBuffer[SIZE_ID_MESSAGE + 1];

    ret = readTimeout(client_fd, msgMessage, SIZE_MSG_MESSAGE,MAX_READ_ATTEMPTS);
    if ( ret == 0)
    {
    	close(client_fd);
        numThreads--; 
        pthread_exit(NULL);        
    }
    if ( ret == -1 && errno == EAGAIN ) 
    {
    	ret = writeCom(client_fd, COMMAND_FINISH_ATTEMPTS); 
        if (ret == -1 && errno == EPIPE)
        {
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);
        }
        if (ret  == -1) 
        {
            errFunction("Error writing to socket");
        }
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);
    }
    if (ret == -1) 
    {
    	errFunction("Error reading from socket"); 
    }


    extractField(msgMessage, objBuffer, 0, SIZE_OBJECT); 
    extractField(msgMessage, textBuffer, SIZE_OBJECT, SIZE_OBJECT + SIZE_TEXT); 
    snprintf(idMsgBuffer, sizeof(idMsgBuffer), "%06d", ++myBoard->idCount); 
    
    lockMutex(mutexBulletinBoard);
    addMessageUser(myBoard, username, objBuffer, textBuffer, idMsgBuffer);
    unlockMutex(mutexBulletinBoard); 
    
    lockMutex(mutexFileMessages); 
    writeMessageFile(username, objBuffer, textBuffer, idMsgBuffer, MESSAGES_FILE); 
    unlockMutex(mutexFileMessages); 

    printUserMessage(myBoard, username);  

    if (writeCom(client_fd, COMMAND_SUCCESS) == -1)
    {
        errFunction("Error writing to socket"); 
    }
}




// The following functions allow viewing messages on the bulletin board. 
// Specifically, viewAllMessageFunction allows viewing all messages, including those authored by others.
// viewMessageFunction allows viewing only messages authored by the user.

static void viewAllMessageFunction(int client_fd, BulletinBoard *myBoard)
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
	
    lockMutex(mutexBulletinBoard); 

    sprintf(numMsgBuff, "%d", myBoard->msgCount);
    padBuff(numMsgBuff, strlen(numMsgBuff), SIZE_NUM_MSG);

    if (writeBuffSocket(client_fd, numMsgBuff, SIZE_NUM_MSG) == -1)
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

	    ret = writeBuffSocket(client_fd, msgMessage, SIZE_GENERIC_COMPLETE_MESSAGE);
	    if (ret == -1 && errno == EPIPE)
	    {
	        close(client_fd);
        	numThreads--; 
        	pthread_exit(NULL);  
	    }
            if (ret == -1)
            {
                errFunction("Error writing to socket");
            }
        }
        currentUser = currentUser->next;
    }
    
    unlockMutex(mutexBulletinBoard); 

    ret = writeCom(client_fd, COMMAND_SUCCESS); 
    if (ret == -1 && errno == EPIPE)
    {
    	close(client_fd);
        numThreads--; 
        pthread_exit(NULL);    
    }
    if (ret == -1)
    {
        errFunction("Error writing to socket");
    }
}



static void viewMessageFunction(int client_fd, BulletinBoard *myBoard, char *username)
{
    int ret;
    char c;
    char numMsgBuff[SIZE_NUM_MSG];
    char msgMessage[SIZE_PERSONAL_COMPLETE_MESSAGE];
    char objBuffer[SIZE_OBJECT + 1];
    char textBuffer[SIZE_TEXT + 1];
    char idMessageBuffer[SIZE_ID_MESSAGE + 1];

    lockMutex(mutexBulletinBoard); 
    
    User *currentUser = findUser(myBoard, username);
    

    sprintf(numMsgBuff, "%d", currentUser->count);
    padBuff(numMsgBuff, strlen(numMsgBuff), SIZE_NUM_MSG);

    ret = writeBuffSocket(client_fd, numMsgBuff, SIZE_NUM_MSG); 
    if (ret  == -1 && errno == EPIPE)
    {
    	close(client_fd);
        numThreads--; 
        pthread_exit(NULL);      
    }
    if (ret  == -1)
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
      

	ret = writeBuffSocket(client_fd, msgMessage, SIZE_PERSONAL_COMPLETE_MESSAGE); 
	if ( ret == -1 && errno == EPIPE)
	{
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);   	
	}
        if ( ret == -1)
        {
            errFunction("Error writing to socket");
        }
    }
	
    unlockMutex(mutexBulletinBoard); 

    ret = writeCom(client_fd, COMMAND_SUCCESS); 
    if (ret == -1 && errno == EPIPE)
    {
    	close(client_fd);
        numThreads--; 
        pthread_exit(NULL);       	
    }
    if (ret == -1)
    {
        errFunction("Error writing to socket");
    }
}




// The following function allows the deletion of a message from the bulletin board based on an ID sent by the client.


static void delMessageFunction(int client_fd, BulletinBoard *myBoard, char *username)
{
    int ret;
    char c;
    char idMessage[SIZE_ID_MESSAGE + 1];
    User *user;

    ret = readTimeout(client_fd, idMessage, SIZE_ID_MESSAGE,MAX_READ_ATTEMPTS);

    if ( ret == 0)
    {
    	close(client_fd);
        numThreads--; 
        pthread_exit(NULL);        
    }
    if ( ret == -1 && errno == EAGAIN ) 
    {
    	ret = writeCom(client_fd, COMMAND_FINISH_ATTEMPTS); 
        if (ret == -1 && errno == EPIPE)
        {
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);
        }
        if (ret  == -1) 
        {
            errFunction("Error writing to socket");
        }
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);
    }
    if (ret == -1) 
    {
    	errFunction("Error reading from socket"); 
    }
  

    idMessage[SIZE_ID_MESSAGE] = 0;

    if (myBoard->idCount < convertStringToNumber(idMessage)) 
    { 
    	ret = writeCom(client_fd, COMMAND_FAILURE); 
    	if (ret  == -1 && errno == EPIPE)
    	{
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);     	
    	}
        if (ret  == -1) 
        {
            errFunction("Error writing to socket");
        }
    } 
    else 
    {
    	lockMutex(mutexBulletinBoard); 
    	
        if (delMessageUser(myBoard, username, idMessage) == -1) 
        {
            if (writeCom(client_fd, COMMAND_FAILURE) == -1) 
            {
                errFunction("Error writing to socket");
            }
        } 
        else 
        {
            lockMutex(mutexFileMessages); 
            delMessageFile(MESSAGES_FILE, idMessage);
            unlockMutex(mutexFileMessages);
            
            ret = writeCom(client_fd, COMMAND_SUCCESS); 
            if (ret == -1 && errno == EPIPE)
            {
            	close(client_fd);
            	numThreads--; 
            	pthread_exit(NULL);   
            }
            if (ret == -1) 
            {
                errFunction("Error writing to socket");
            }
        }
        
        unlockMutex(mutexBulletinBoard); 
    }
}





// The authFuncServer function allows the client to authenticate: register a new account or log in to an existing one.
// It makes use of the auxiliary functions subFunctionServer and logFunctionServer.

static void subFunctionServer(int client_fd, BulletinBoard *myBoard, char *username, char *password)
{
    int ret;
    char authMessage[SIZE_AUTH_MESSAGE];

    while (1) 
    {
        ret = readTimeout(client_fd, authMessage, SIZE_AUTH_MESSAGE, MAX_READ_ATTEMPTS);
        
        if ( ret == 0)
        {
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);        
        }
        if ( ret == -1 && errno == EAGAIN ) 
        {
            ret = writeCom(client_fd, COMMAND_FINISH_ATTEMPTS); 
            if (ret == -1 && errno == EPIPE)
            {
            	close(client_fd);
            	numThreads--; 
            	pthread_exit(NULL);
            }
            if (ret  == -1) 
            {
                errFunction("Error writing to socket");
            }
            close(client_fd);
            numThreads--; 
            pthread_exit(NULL);
        }
        if (ret == -1) 
        {
           errFunction("Error reading from socket"); 
        }

	
        extractField(authMessage, username, 0, SIZE_USERNAME);
        extractField(authMessage, password, SIZE_USERNAME, SIZE_USERNAME + SIZE_PASSWORD);
        

        
        lockMutex(mutexBulletinBoard); 
        
        if (findUser(myBoard, username)) 
        {
            ret = writeCom(client_fd, COMMAND_ERR_USER_ALREADY_EXISTS); 
            if ( ret == -1 && errno == EPIPE)
            {
          	close(client_fd); 
    	    	numThreads--; 
    	    	pthread_exit(NULL);             
            }
            if ( ret == -1 ) 
            {
                errFunction("Error writing to socket");
            }
        } 
        else 
        {
            ret = writeCom(client_fd, COMMAND_SUCCESS) ;
            if ( ret == -1 && errno == EPIPE)
            {
          	close(client_fd); 
    	    	numThreads--; 
    	    	pthread_exit(NULL);                
            }
            if ( ret == -1) 
            {
                errFunction("Error writing to socket");
            }
            unlockMutex(mutexBulletinBoard); 
            break;
        }
        
        unlockMutex(mutexBulletinBoard); 
    }

    lockMutex(mutexBulletinBoard);
    addUser(myBoard, username, password);
    unlockMutex(mutexBulletinBoard); 
    
    lockMutex(mutexFileUsers); 
    writeUserFile(username, password, USERS_FILE);
    unlockMutex(mutexFileUsers); 
}



static void logFunctionServer(int client_fd, BulletinBoard *myBoard, char *username, char *password)
{
    char authMessage[SIZE_AUTH_MESSAGE];
    int ret;

    while (1) 
    {
        ret = readTimeout(client_fd, authMessage, SIZE_AUTH_MESSAGE, MAX_READ_ATTEMPTS);
	if ( ret == 0)
	{
          	close(client_fd); 
    	    	numThreads--; 
    	    	pthread_exit(NULL);    	
	}
        if ( (ret == -1 && errno == EAGAIN )) 
        {
            ret = writeCom(client_fd, COMMAND_FINISH_ATTEMPTS); 
            if (ret == -1 && errno == EPIPE)
            {
            	close(client_fd);
            	numThreads--;
            	pthread_exit(NULL);            
            }
            if (ret  == -1) 
            {
                errFunction("Error writing to socket");
            }
            close(client_fd);
            numThreads--;
            pthread_exit(NULL);
        }
        if (ret == -1) 
        {
           errFunction("Error reading from socket"); 
        }

	
        extractField(authMessage, username, 0, SIZE_USERNAME);
        extractField(authMessage, password, SIZE_USERNAME, SIZE_USERNAME + SIZE_PASSWORD);
        
	
	lockMutex(mutexBulletinBoard); 
	
        if (findUser(myBoard, username) == NULL) 
        {
            ret = writeCom(client_fd, COMMAND_ERR_USER_NOT_FOUND); 
            if (ret == -1 && errno == EPIPE)
            {
            	close(client_fd); 
    	    	numThreads--; 
    	    	pthread_exit(NULL);             
       	    
            }
            if (ret == -1) 
            {
                errFunction("Error writing to socket");
            }
            unlockMutex(mutexBulletinBoard); 
            continue;
        }
        if (checkUserPass(myBoard, username, password)) 
        {
            ret = writeCom(client_fd, COMMAND_ERR_NOT_MATCH_CREDENTIALS); 
            if (ret == -1 && errno == EPIPE)
            {
            	close(client_fd); 
    	    	numThreads--; 
    	    	pthread_exit(NULL); 
            }
            if (ret == -1) 
            {
                errFunction("Error writing to socket");
            }
        } 
        else 
        {
            ret = writeCom(client_fd, COMMAND_SUCCESS); 
            if (ret == -1 && errno == EPIPE)
            {
            	close(client_fd); 
    	    	numThreads--; 
    	    	pthread_exit(NULL);                
            }
            if (ret == -1) 
            {
                errFunction("Error writing to socket");
            }
            unlockMutex(mutexBulletinBoard); 
            break;
        }
       
    }
}




static void authFuncServer(int client_fd, BulletinBoard *myBoard, char *username, char *password)
{
    char c;
    int ret;
	
    ret = writeCom(client_fd, COMMAND_AUTH); 
    if( ret == -1 && errno == EPIPE)
    {
    	close(client_fd); 
    	numThreads--; 
    	pthread_exit(NULL);     
    }
    if (ret ==-1) 
    {
        errFunction("Error writing to socket");
    }

    ret = readCom(client_fd, &c);
    if (ret == -1) 
    {
        errFunction("Error reading from socket");
    }else if(ret == 0)
    {
    	close(client_fd); 
    	numThreads--; 
    	pthread_exit(NULL); 
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
    	ret = writeCom(client_fd, COMMAND_CLOSE);
    	if (ret == -1 && errno == EPIPE)
    	{
            close(client_fd);  
            numThreads--;              
            pthread_exit(NULL);      	
    	}
        if (ret == -1) 
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


    lockMutex(mutexFileUsers);
    authFuncServer(client_fd, myBoard, username, password);       

    while(1){        
        ret = readCom(client_fd, &c);        
        if(ret == 0)        
        {   
       	    close(client_fd); 
       	    numThreads--;         
            pthread_exit(NULL);  	        
        } else if( ret == -1)        
        {            
            errFunction("Error reading from socket");         
        }                         

        switch(c){            
            case COMMAND_QUIT:   
            	close(client_fd);  
            	numThreads--;              
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
            	ret = writeCom(client_fd, COMMAND_CLOSE); 
            	if( ret == -1 && errno == EPIPE)
            	{
            	    close(client_fd);  
            	    numThreads--;              
                    pthread_exit(NULL);   
            	}               
                if( ret == -1)                
                {                     
                    errFunction("Error writing to socket");                 
                }                 
                close(client_fd);
                numThreads--;                
                pthread_exit(NULL);         
        }	     
    } 
}

