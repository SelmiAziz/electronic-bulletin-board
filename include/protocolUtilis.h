#ifndef PROTOCOL_UTILIS_H
#define PROTOCOL_UTILIS_U

int readCom(int socket, char *command);
int writeCom(int socket, char command);
int writeBuffSocket(int socket, char *buffer, int len);




#endif
