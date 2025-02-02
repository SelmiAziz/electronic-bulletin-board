#ifndef FUNCTION_CLIENT_H
#define FUNCTION_CLIENT_H

#define SIZE 1024
#include "commServerClientConfig.h"
#include "utilityForFunctionClient.h"

void clientFunc(int socket); 
int takeArgumentsClient(int argc, char **argv, char **serverAddress, short int *port); 


#endif
