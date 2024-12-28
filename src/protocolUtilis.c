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
int readTimeout(int fd, void *buffer, int total_bytes, int max_attempts)
{
    int bytes_read = 0;
    int result;
    int attempts = 0;

    while (bytes_read < total_bytes) {
        result = read(fd, (char *)buffer + bytes_read, total_bytes - bytes_read);

        if (result < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                attempts++;
                if (attempts == max_attempts) {
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


int readBuffSocket(int fd, void *buffer, int total_bytes)
{
    int bytes_read = 0;
    int result;
    int attempts = 0;

    while (bytes_read < total_bytes) {
        result = read(fd, (char *)buffer + bytes_read, total_bytes - bytes_read);

        if (result < 0) {
            return -1; 
        } else if (result == 0) {
            return 0; 
        }

        bytes_read += result;
    }

    return bytes_read; 
}


