#include<stdio.h>
#include<string.h>
#include"helper.c"


#define MAX_USERNAME 64
#define MAX_PASSWORD 64




int checkFormatUsername(char *username)
{
	if(strlen(username) > 4) return 0; 
	return -1; 

}

int getValideUsername(char *username)
{
	while(1){
		if(getInput(username, MAX_USERNAME))
		{
			errFunction("Errore di lettura username da stdin"); 
		}
		if(checkFormatUsername(username) == 0) break;
		printf("Lunghezza username deve essere >4"); 
	}
	return 0; 

}


int controlPass(char *password, char *seq){
	for(int i = 0; i<strlen(password); i++){
		for(int j = 0; j<strlen(seq); j++){
			if(password[i] == seq[j]){
				return 0; 
			}
		}
	}
	return -1; 

}
int checkFormatPassword(char *password){
	char *specials = "!?#$%^&*()-+_="; 
	char *num = "1234567890"; 
	char *alM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	if(strlen(password) < 8) return -1; 
	if(controlPass(password, specials) || controlPass(password,num) || controlPass(password, alM)) return -1;
	return 0; 


}

int getValidePassword(char *password){
	while(1){
		if(getInput(password, MAX_PASSWORD))
		{
			errFunction("Errore di lettura password da stdin");
		}
		if(checkFormatPassword(password) == 0) break; 
		printf("Formato password :almeno 8 caratteri tra cui una lettera maiuscola, un numero e un carattere speciale\n"); 
	}

	return 0; 
}

void buildAuthMessage(char *unis, char *s1, char *s2)
{
	sprintf(unis, "%s %s", s1, s2); 
	unis[MAX_USERNAME+MAX_PASSWORD+3] = 0; 
}

void extractAuthMessage(char *s, char *username, char *password) {
    int i, l = 0; 
    for(i = 0; i < MAX_USERNAME && s[i] != ' '; i++) {
        username[i] = s[i]; 
    }
    username[i] = '\0'; 
    printf("Il numero di partenza è %d\n", MAX_USERNAME); 
  	for(i = MAX_USERNAME+1; s[i] != ' ' && s[i] != '\0' ; i++){
 		password[l] = s[i]; 
 		l++; 
  	}
  	password[l] = '\0'; 
}

void padBuff(char *buffer, int len, int maxLen){
	if(len<maxLen)
	{
		memset(buffer + len, ' ', maxLen-len); 
	}
	buffer[maxLen-1] = '\0'; 
}

int main(int arcv, char *argv)
{
	char username[MAX_USERNAME+1]; 
	char password[MAX_PASSWORD+1]; 
	char new_username[MAX_USERNAME+1]; 
	char new_password[MAX_PASSWORD+1]; 
	char uniS[MAX_USERNAME + MAX_PASSWORD +2]; 
	printf("Inserire username\n");
	getValideUsername(username); 
	padBuff(username, strlen(username), MAX_USERNAME+1); 
	printf("La lunghezza della password è %d\n", strlen(username)); 
	printf("La password é %s\n", username); 
	printf("Inserire password\n"); 
	getValidePassword(password); 
	padBuff(password, strlen(password), MAX_PASSWORD+1); 
	printf("La lunghezza della password é %d\n", strlen(password)); 
	printf("Password %s\n", password); 
	createMessage(uniS,username, password); 
	printf("IL risultato é %s\n", uniS);
	printf("Lunghezza di tutto %d\n", strlen(uniS)); 
	takeUserPass(uniS, new_username, new_password); 
	printf("Lo username è %s con lunghezza %d\n", new_username, strlen(new_username)); 
	printf("La password é %s\n", new_password); 












	return 0;
}
