#include <sys/socket.h>       
#include <sys/types.h>        
#include <arpa/inet.h>        
#include <unistd.h>        
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/functionClient.h"
#include "../include/helper.h"


//ATTENZIONE CLIENT HAS STILL A NOT GOOD READ BUFFER FUNCTION
int main(int argc, char *argv[])
 {
    int       connSocket; 
    struct    sockaddr_in serverAddress; 
    struct    sigaction sa_ignore; 
    char     *serverIp;                        
    short int port; 


    takeArgumentsClient(argc, argv, &serverIp, &port); 
    

    if ( (connSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		errFunction("Errore in socket creation"); 
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_port        = htons(port);

    //conversione in formato binario
    if (inet_pton(AF_INET, serverIp, &serverAddress.sin_addr) <= 0) {
     	errFunction("Error in conversoion ip address"); 
    }

    if ( connect(connSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress) ) < 0 ) {
		errFunction("Error in connect"); 
     }
     
     
     clientFunc(connSocket); 
     
    return 0; 
}

