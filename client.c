#include <sys/socket.h>       
#include <sys/types.h>        
#include <arpa/inet.h>        
#include <unistd.h>        
#include <netinet/in.h>
#include <netdb.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "functionsClient.h"
#include "helper.h"

int main(int argc, char *argv[]) {
    int       connSocket; 
    struct    sockaddr_in serverAddress; 
    char     *serverIp;                        
	short int port; 


	takeArgumentsClient(argc, argv, &serverIp, &port); 
	

    if ( (connSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		errFunction("Errore nella creazione della socket"); 
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_port        = htons(port);


 	 if (inet_pton(AF_INET, serverIp, &serverAddress.sin_addr) <= 0) {
     	errFunction("Errore conversione indrizzo ip"); 
    }

    if ( connect(connSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress) ) < 0 ) {
		errFunction("Errore nella connect"); 
     }
     
     
     clientFunc(connSocket); 
     
    return 0; 
}

