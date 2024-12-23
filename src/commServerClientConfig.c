#include<unistd.h>
#include<errno.h>

#include"commServerClientConfig.h"

//NON MI CONVINCE CHE QUESTE FUNZIONI STIANO QUI!!!

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
