#ifndef UTILITY_FOR_FUNCTION_SERVER_H
#define UTILITY_FOR_FUNCTION_SERVER_H

void extract2Message(char *s, char *str1, char *str2, int l1, int l2);
void buildPersonalUserMessage(char *msgMessage, char *object, char *text, char *idMessage);
void buildGenericUserMessage(char *msgMessage, char *object, char *text, char *idMessage, char *author);
void copyInBuffer(char *buff, char *origin);

#endif
