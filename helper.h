void errFunction(char *err); 
int takeArgumentsClient(int argc, char **argv, char **serverAddress, short int *port); 
int getInput(char *buffer, int maxLen);
void fill(char *l, char *buffUser, char *buffPass); 
void fillMsg(char *l, char *buffUser, char *buffObj, char *buffText, int *v);  
int wrUser(char *buffUser, char *buffPassword, char *file); 
void wrMessage(char *buffUser, char *buffObj, char *buffText, char *file); 
