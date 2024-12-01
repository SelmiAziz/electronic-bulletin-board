
//from messageLib.c
void delMessageFile(FILE *f, char *idMessage); 
//the implemantation is from helper.c
void fillUser(char *bufferLine, char *buffUsername, char *buffPassword); 
void fillMsg(char *bufferLine, char *buffObject, char *buffText, char *buffIdMessage, int *flagV);
void writeUserFile(char *buffUsername, char *buffPaswordm, char *file); 
void writeMessageFile(char **buffUsername, char *bufferObject, char *buffText, char *buffIdMessage, char *file); 
