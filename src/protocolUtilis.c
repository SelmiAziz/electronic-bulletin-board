#include<errno.h>
#include<unistd.h>

#include "../include/protocolUtilis.h"

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
//deve essere sviluppata una funzione read per il client che non abbia Timeout
int readTimeout(int fd, void *buffer, int totalBytes, int maxAttempts)
{
    int bytesRead = 0;
    int result;
    int attempts = 0;

    while (bytesRead < totalBytes) {
        result = read(fd, (char *)buffer + bytesRead, totalBytes - bytesRead);

        if (result < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                attempts++;
                if (attempts == maxAttempts) {
                    return -1;
                }
                continue; 
            }
            return -1; 
        } else if (result == 0) {
            return 0; 
        }

        bytesRead += result;
    }

    return bytesRead; 
}


int readBuffSocket(int fd, void *buffer, int totalBytes)
{
    int bytesRead = 0;
    int result;
    int attempts = 0;

    while (bytesRead < totalBytes) {
        result = read(fd, (char *)buffer + bytesRead, totalBytes - bytesRead);

        if (result < 0) {
            return -1; 
        } else if (result == 0) {
            return 0; 
        }

        bytesRead += result;
    }

    return bytesRead; 
}


