void delMessageFile(char *file, char *idMessage);

void fillUser(char *bufferLine, char *buffUsername, char *buffPassword);
void fillMsg(char *buffLine, char *buffUsername, char *buffObject, char *buffText, char *buffIdMessage, int *presenceBit);

void writeUserFile(char *buffUser, char *buffPassword, char *file);
void writeMessageFile(char*buffUser, char *buffObj, char *buffText, char *buffIdMessage, char *file);

void copyFile(const char* fileSource, const char* fileDestination); 
void eliminateZeroPresence(char *file, char *fileShadow);
