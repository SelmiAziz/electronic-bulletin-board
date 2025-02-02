#ifndef PROTOCOL_UTILIS_H
#define PROTOCOL_UTILIS_U

int readCom(int socket, char *command);
int writeCom(int socket, char command);
int writeBuffSocket(int socket, char *buffer, int len);
int readTimeout(int fd, void *buffer, int total_bytes, int maxAttempts);
int readBuffSocket(int fd, void *buffer, int totalBytes);


#endif
