#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>

#include<pthread.h>
#include <unistd.h>   

#include"functionServer.h"
#include"helper.h"


int readCom(int socket, char *command)
{
	return read(socket, command, 1); 
}

int writeCom(int socket, char command)
{
	return write(socket, &command, 1); 
}

int writeBuffSocket(int socket, char *buffer, int len)
{
	int l,w; 
	l = len; 
	while(l > 0)
	{

		w = write(socket, buffer, l); 
		if( w <= 0)
		{
			if(errno == EINTR) w = 0; 
			else{
				return -1; 
			}
		}
		l -= w; 
		buffer += w; 
		
	}
	return len; 
}


//la funzione ritorna -1 errno invariato errore di lettura
//la funzione ritorna 0 canale chiuso
//la funzione ritorna -1 errno uguale a EWOULDBLOCK oppure EAGAIN errore di timeout
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

//pure unpadding potrei tranquillamente metterla insieme a helper.c o cose del genere
void unpadding(char *buff, int len)
{
	for(int i = strlen(buff)-1; i>0; i--){
		if(buff[i] != ' ') break; 
		buff[i] = '\0'; 
	}

}

void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen)
	{
		memset(buffer + len, ' ', maxLen-len); 
	}
	buffer[maxLen] = 0; 
}


//questa funzione potrebbe tranquillamante andare con un file con buil e padding ste cose qua
void extract2Message(char *s, char *str1, char *str2, int l1, int l2) 
{
    int i, l = 0; 
  
    for(i = 0; i < l1 && !(s[i] == ' ' && s[i+1] == ' '); i++)
    {
        str1[i] = s[i]; 
    }
    str1[i] = '\0'; 

    
  	for(i = l1 ; i < l1 + l2 && !(s[i] == ' ' && s[i+1] == ' ') ; i++)
  	{
 		str2[l] = s[i]; 
 		l++; 
  	}
  	str2[l] = '\0'; 
}

int findEndField(char *f, int endMax)
{ 
	for(int i = endMax; i >= 0; i--)
	{
		if(f[i] == ' ') return i; 
	}
	return endMax; 

}


void extractField(char *s,char *str,  int lStart, int lMax)
{
	int i ; 
	int l = 0; 
	int lEnd = findEndField(s,lMax); 
	for( i = lStart; i < lEnd && !(s[i] == ' ' && s[i+1] == ' '); i++)
	{
		//per adesso mi accontento di questo ma questa funazione è da migliorare perché deve prendere tutta la stringa fino a quando effettiviamente stanno solo spazi
		str[l] = s[i]; 
		l++;
	}
	str[l] = '\0'; 
}


static void subFunctionServer(int socket,BulletinBoard *myBoard, char *username, char *password)
 {
    int ret; 
    char authMessage[SIZE_AUTH_MESSAGE]; 
    
    while (1) 
    {
       ret = readTimeout(socket, authMessage, SIZE_AUTH_MESSAGE);  
       
        if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)))
         {
            if(writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS) == -1)
            {
            	errFunction("Errore di scrittura sulla socket"); 
            }
            close(socket);
            pthread_exit(NULL); 
        }
        if (ret == -1) {
            if(writeCom(socket, COMMAND_CLOSE) == -1)
            {
            	errFunction("Errore di scrittura sulla socket"); 
            }
            close(socket);
            pthread_exit(NULL); 
        }
        
    	extractField(authMessage,username,0,SIZE_USERNAME); 
    	extractField(authMessage,password,SIZE_USERNAME, SIZE_USERNAME + SIZE_PASSWORD); 
   
   
        if (findUser(myBoard, username)) 
        {
            if(writeCom(socket, COMMAND_ERR_USER_ALREADY_EXISTS) == -1)
            {
            	errFunction("Errore di scrittura sulla socket");
            }
        } else {
            if(writeCom(socket, COMMAND_SUCCESS) == -1)
            {
            	errFunction("Errore di scrittura sulla socket"); 
            }
            break;
        }
    }
    addUser(myBoard, username, password); 
    wrUser(username, password, "back_users.csv"); 
    sleep(5); 
}


static void logFunctionServer(int socket, BulletinBoard *myBoard, char *username, char *password)
{
	//domanda ma nel buffer ci sono solo i caratteri o metto l'operatore finale
	char authMessage[SIZE_AUTH_MESSAGE]; 
	int ret;  
	
	while(1)
	{
		ret = readTimeout(socket, authMessage, SIZE_AUTH_MESSAGE); 
		
		if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
		{
    		if(writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS) == -1)
    		{
    			errFunction("Errore di scrittura sulla socket"); 
    		}
        	close(socket);
       		pthread_exit(NULL); 
		}
		if (ret == -1) 
		{
			if(writeCom(socket, COMMAND_CLOSE) == -1)
			{
				errFunction("Errore di scrittura sulla socket"); 
			}
			
		  	close(socket);
		  	pthread_exit(NULL); 
		}
		
		
		extractField(authMessage,username,0,SIZE_USERNAME); 
    	extractField(authMessage,password,SIZE_USERNAME, SIZE_USERNAME + SIZE_PASSWORD);	
		
		
		if(findUser(myBoard,username) == NULL)
		{
			if(writeCom(socket,COMMAND_ERR_USER_NOT_FOUND) == -1)
			{
				errFunction("Errore di scrittura sulla socket");
			}
			
			continue; 
		}
		if(checkUserPass(myBoard, username, password))
		{
			if(writeCom(socket, COMMAND_ERR_NOT_MATCH_CREDENTIALS) == -1)
			{
				errFunction("Errore di scrittura sulla socket"); 
			}
		}else{
			if(writeCom(socket,COMMAND_SUCCESS) == -1)
			{
				errFunction("Errore di scrittura sulla socket"); 
			}
			
			break; 
		}
	}
}




static void authFuncServer(int socket, BulletinBoard *myBoard, char *username, char *password)
{	
	char c; 
	int ret; 
	//meglio così che è brutto che il client inizi a manda roba a valanga
	//ci sarebbe da cambiare tutte le funzioni devono essere delle int
	if (writeCom(socket, COMMAND_AUTH) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	}

	ret = readCom(socket, &c); 
	
	if(ret == 0)
	{
		//il client ha chiuso quindi butto giù il thread no ?
		pthread_exit(NULL); 
	}else if(ret == -1)
	{
		errFunction("Errore di lettura dalla socket"); 
	}
	//non dovrebbe mandare qulcosa per dire al client che è andato a buon fine il tutto ?
	if( c == COMMAND_SUB)
	{
			subFunctionServer(socket, myBoard, username, password); 
			logFunctionServer(socket, myBoard, username, password);
			 
	}else if( c == COMMAND_LOG)
	{
			logFunctionServer(socket, myBoard, username, password); 
			
	}else{
		//mi hai mandato roba che non era vera
		if (writeCom(socket, COMMAND_CLOSE) == -1)
		{
			errFunction("Errore di scrittura sulla socket"); 
		}
	}
}


static int postMessageFunction(int socket, BulletinBoard *myBoard, char *username)
{
	int ret; 
	char c; 
	char objBuffer[SIZE_OBJECT+1]; 
	char textBuffer[SIZE_TEXT+1]; 
	char msgMessage[SIZE_MSG_MESSAGE]; 
	
 	ret = readTimeout(socket, msgMessage,  SIZE_MSG_MESSAGE); 
 	
    if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
    {
    	writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
        close(socket);
        pthread_exit(NULL); 
    }
    if (ret == -1) 
    {
    	if(writeCom(socket, COMMAND_CLOSE) == -1)
    	{
    		errFunction("Errore di scrittura sulla socket"); 
    	}
        close(socket);
        pthread_exit(NULL); 
    }
	
	extractField(msgMessage,objBuffer, 0, SIZE_OBJECT); 
	extractField(msgMessage,textBuffer,SIZE_OBJECT, SIZE_OBJECT+SIZE_TEXT); 
	
	//use add MEssage User
	//qua di dovrebbero fare dei controlli così non mi piace affatto
	//!!!!!ERRRORE DI RESPONSABILITY
	addMessageUser(myBoard,username,objBuffer, textBuffer, "back_messages.csv");
	
	printUserMessage(myBoard, username);  


	if(writeCom(socket, COMMAND_SUCCESS) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	}
}

void buildPersonalUserMessage(char *msgMessage, char *object, char *text, char *idMessage)
{
	sprintf(msgMessage,"%s%s%s",object,text,idMessage);
}


void buildGenericUserMessage(char *msgMessage, char *object, char *text, char *idMessage, char *author)
{
	sprintf(msgMessage,"%s%s%s%s",object,text,idMessage,author);
}


//il nome non mi convince
void copyInBuffer(char *buff, char *origin)
{
	strncpy(buff, origin, strlen(origin)); 
	buff[strlen(origin)] = 0; 

}

static void viewAllMessageFunction(int socket, BulletinBoard *myBoard)
{
	int ret; 
	char c; 
	char numMsgBuff[SIZE_NUM_MSG];
	char msgMessage[SIZE_GENERIC_COMPLETE_MESSAGE]; 
	char objBuffer[SIZE_OBJECT+1]; 
	char textBuffer[SIZE_TEXT+1]; 
	char idMessageBuffer[SIZE_MESSAGE_ID+1]; 
	char authorBuffer[SIZE_USERNAME+1]; 
	User *currentUser; 
	
	
	sprintf(numMsgBuff, "%d\0", myBoard->msgCount); 
	padBuff(numMsgBuff, strlen(numMsgBuff), SIZE_NUM_MSG); 
	
	
	if(writeBuffSocket(socket,numMsgBuff, SIZE_NUM_MSG) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	
	}
	
	
	currentUser = myBoard->head; 
	while(currentUser)
	{
		for(int i = 0; i<currentUser->count;i++)
		{
		
			copyInBuffer(objBuffer,currentUser->messages[i]->object); 
			copyInBuffer(textBuffer,currentUser->messages[i]->text);
			copyInBuffer(idMessageBuffer,currentUser->messages[i]->idMessage);
			copyInBuffer(authorBuffer, currentUser->username);
			
			
			
			padBuff(objBuffer, strlen(objBuffer), SIZE_OBJECT); 
			padBuff(textBuffer, strlen(textBuffer), SIZE_TEXT); 
			padBuff(idMessageBuffer, strlen(idMessageBuffer), SIZE_MESSAGE_ID); 
			padBuff(authorBuffer, strlen(authorBuffer), SIZE_USERNAME); 
			
			
			
		
			buildGenericUserMessage(msgMessage, objBuffer, textBuffer, idMessageBuffer, authorBuffer);


			if(writeBuffSocket(socket, msgMessage, SIZE_GENERIC_COMPLETE_MESSAGE) == -1)
			{
				errFunction("Errore di scrittura sulla socket"); 
				fflush(stdout); 
			}
		}
		currentUser = currentUser->next;
	
	
	
	
	}
	fflush(stdout); 

}

static void viewMessageFunction(int socket, BulletinBoard *myBoard, char *username)
{
	
	int ret; 
	char c; 
	char numMsgBuff[SIZE_NUM_MSG];
	char msgMessage[SIZE_PERSONAL_COMPLETE_MESSAGE]; 
	char objBuffer[SIZE_OBJECT+1]; 
	char textBuffer[SIZE_TEXT+1]; 
	char idMessageBuffer[SIZE_MESSAGE_ID+1]; 
	
	//forse quuesta potrebbe tornnare NULL e fare un controllo boh
	User *currentUser = findUser(myBoard,username); 

	
	//questo è da sistemare e quando è finito lo spazio per i messaggi fai alla funzione di messageLib che restituisce meno -1 così glielo diciamo che non abbiamo spazio
	sprintf(numMsgBuff,"%d\0",currentUser->count);
	padBuff(numMsgBuff,strlen(numMsgBuff), SIZE_NUM_MSG);
	

	
	if(writeBuffSocket(socket,numMsgBuff, SIZE_NUM_MSG) == -1)
	{
		errFunction("Errore di scrittura sulla socket"); 
	
	}
	
	//forse è meglio implementare una funzione per fare il copia
	
	for(int i = 0; i<currentUser->count;i++)
	{
	
		copyInBuffer(objBuffer,currentUser->messages[i]->object); 
		copyInBuffer(textBuffer,currentUser->messages[i]->text);
		copyInBuffer(idMessageBuffer,currentUser->messages[i]->idMessage);
		
		
		
		padBuff(objBuffer, strlen(objBuffer), SIZE_OBJECT); 
		padBuff(textBuffer, strlen(textBuffer), SIZE_TEXT); 
		padBuff(idMessageBuffer, strlen(idMessageBuffer), SIZE_MESSAGE_ID); 
		
		
		
		
	
		buildPersonalUserMessage(msgMessage, objBuffer, textBuffer, idMessageBuffer);


		if(writeBuffSocket(socket, msgMessage, SIZE_PERSONAL_COMPLETE_MESSAGE) == -1)
		{
			errFunction("Errore di scrittura sulla socket"); 
		}
	}


	//qui poi fare un controllo e inviare COMMAND_SUCCESS per dire tutto bene oppure una flag per dire che non ci sono messaggi
	//if(writeCom(socket, COMMAND_SUCCESS) == -1)
	{
		//errFunction("Errore di scrittura sulla socket"); 
	
	}
	fflush(stdout); 
}


static void delMessageFunction(int socket, BulletinBoard *myBoard, char *username)
{
	int ret; 
	char c; 
	char idMessage[SIZE_MESSAGE_ID+1]; 
	User *user; 
	
	ret = readTimeout(socket, idMessage, SIZE_MESSAGE_ID); 
	
	if (ret == 0 || (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))) 
    {
    	writeCom(socket, (ret == 0) ? COMMAND_CLOSE : COMMAND_FINISH_ATTEMPTS);
        close(socket);
        pthread_exit(NULL); 
    }
    if (ret == -1) 
    {
    	if(writeCom(socket, COMMAND_CLOSE) == -1)
    	{
    		errFunction("Errore di scrittura sulla socket"); 
    	}
        close(socket);
        pthread_exit(NULL); 
    }
    
    idMessage[SIZE_MESSAGE_ID] = 0; 
    
    
    printf("Messaggio richiesto di eliminare è %s\n", idMessage); 
    fflush(stdout); 
    
   
    
    if(myBoard->msgCount< strtol(idMessage, NULL, 10 ) )
    {
    	printf("qo"); 
    	fflush(stdout); 
    	if( writeCom(socket, COMMAND_FAILURE) == -1)
    	{
    		errFunction("Errore di scrittura sulla socket"); 
    	}
    }else{
    	if(delMessageUser(myBoard, username, idMessage) == -1)
    	{
    		printf("qa!"); 
    		fflush(stdout); 
    		if (writeCom(socket, COMMAND_FAILURE) == -1)
    		{
    			errFunction("Errore di scrittura sulla socket"); 
    		}
    	}else
    	{
    		if(writeCom(socket, COMMAND_SUCCESS) == -1)
    		{
    			errFunction("Errore di scrittura sulla socket"); 
    		}
    	
    	}
    
    }
	





}

void *worker(void *arg)
{
	ThreadData *tData = (ThreadData*) arg; 
	char username[SIZE_USERNAME+1]; 
	char password[SIZE_PASSWORD+1]; 
	char c; 
	int ret; 
	int socket = tData->socket; 
	BulletinBoard *myBoard = tData->myBoard; 
	
	free(tData); 
	tData == NULL; 
	

	authFuncServer(socket, myBoard, username, password);
	
	while(1){
		ret = readCom(socket, &c);
		if(ret == 0)
		{
			//qua si dovrebbe fare un qualcosa tipo 
			pthread_exit(NULL);  	
		}else if( ret == -1)
		{
			errFunction("Errore di lettura dal socket"); 
		}
				
		switch(c){
			case COMMAND_QUIT: 
				pthread_exit(NULL); 
			break; 
		
			case COMMAND_VIEW_MSG: 
			//funzione che manda tutti gli oggetti
			//dopo manda un COMMAND_SUCCESS oppure prima mando il numero (come lo esprimo il numero scusa oppure mando mando fin quando alla fine mando success per dire che ho finito)
			//ma se gli manda le stringhe che rappresentano i messaggi e i testi come fa a tenerli dentro il sistema il client, costruisce una struttura oppure printa tutto al volo?
			//meglio mischiarli entrambi
			//riceve una stringa che è fatta padding per indicare il messaggio in questione
		
			//posso chiedere più volte di vedere un messaggio
				viewMessageFunction(socket,myBoard,username);	
			break; 
			
			case COMMAND_VIEW_ALL_MSG: 
				viewAllMessageFunction(socket,myBoard); 
			break; 
			case COMMAND_POST_MSG: 
				//attendo n byte per il oggetto del messaggio
				//attendo m byte per il testo messaggio
				//mand un success
				postMessageFunction(socket, myBoard, username); 
			break; 
			case COMMAND_DELETE_MSG: 
				//ricevo una stringa che indica id-messaggio
				//lo elmino subito oppure aspetto per eliminarlo ?
				//faccio cancellazione in base all'username
				//se non lo puoi cancellare allora mandi una flag per dire che tutto è andato male
				delMessageFunction(socket, myBoard, username); 
				//elminato mando un success
			break; 
			default: 
				if(writeCom(socket, COMMAND_CLOSE) == -1)
				{
					errFunction("Errore di scrittura sulla socket"); 
				}
		
		}	
	}
}
