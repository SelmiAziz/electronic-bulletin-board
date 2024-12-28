#ifndef UTILITY_FOR_FUNCTION_CLIENT_H
#define UTILITY_FOR_FUNCTION_CLIENT_H

int checkFormatUsername(char *username);
void getValideUsername(char *username);
int controlPass(char *password, char *seq);
int checkFormatPassword(char *password);
void getValidePassword(char *password);
void buildMessage(char *bMessage, char *s1, char *s2, int len);
int numberMessages(int socket);







#endif
