#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>

#include "../include/fileMessageLib.h"
#include "../include/helper.h"
#include "../include/commServerClientConfig.h"

#define SIZE 1024


// Helper function that copies a token without quotes from the source to the destination buffer.
// It is designed to handle how messages are represented in the CSV backup file.
void static copyTokenWithoutQuotes(char *destBuff, char *strToken, int lenMax)
{
    char buffTemp[lenMax];
    int len = strlen(strToken);

    if (len > lenMax)
    {
        errFunction("Error: Token length exceeds the allowed maximum size.");
    }

    strncpy(buffTemp, strToken, len);
    removeQuotes(buffTemp);

    len = len - 2;
    strncpy(destBuff, buffTemp, len);
    destBuff[len] = '\0';
}

// This function fills the message fields from a line read from the message file.
// It extracts object, text, username, message ID, and the presence flag.
void fillMsg(char *buffLine, char *buffUsername, char *buffObject, char *buffText, char *buffIdMessage, int *presenceBit)
{
    char buffTemp[SIZE];
    char *token;
    int len;

    strncpy(buffTemp, buffLine, SIZE - 1);
    buffTemp[SIZE - 1] = '\0';

    token = strtok(buffTemp, ",");
    if (token == NULL)
    {
        errFunction("Error: No token found for object field.");
    }
    copyTokenWithoutQuotes(buffObject, token, SIZE_OBJECT + 2);

    token = strtok(NULL, ",");
    if (token == NULL)
    {
        errFunction("Error: No token found for text field.");
    }
    copyTokenWithoutQuotes(buffText, token, SIZE_TEXT + 2);

    token = strtok(NULL, ",");
    if (token == NULL)
    {
        errFunction("Error: No token found for username field.");
    }
    copyTokenWithoutQuotes(buffUsername, token, SIZE_USERNAME + 2);

    token = strtok(NULL, ",");
    if (token == NULL)
    {
        errFunction("Error: No token found for message ID field.");
    }
    copyTokenWithoutQuotes(buffIdMessage, token, SIZE_ID_MESSAGE + 2);

    token = strtok(NULL, ",");
    if (token == NULL)
    {
        errFunction("Error: No token found for presence flag field.");
    }
    *presenceBit = convertStringToNumber(token);
    if (*presenceBit == -1)
    {
        errFunction("Error: Failed to convert presence flag.");
    }
}

// This helper function copies a token from the source to the destination buffer.
// It does not modify the token if it is shorter than the maximum length.
void static copyToken(char *destBuff, char *strToken, int lenMax)
{
    int len = strlen(strToken);

    if (len > lenMax)
    {
        errFunction("Error: Token length exceeds the allowed maximum size.");
    }

    strncpy(destBuff, strToken, len);
    destBuff[len] = '\0';
}

// This function fills user details such as username and password from the file line.
void fillUser(char *bufferLine, char *buffUsername, char *buffPassword)
{
    char buffTemp[SIZE];
    char *token;
    int len;

    strncpy(buffTemp, bufferLine, SIZE - 1);
    buffTemp[SIZE - 1] = '\0';

    token = strtok(buffTemp, "\"");
    if (token == NULL)
    {
        errFunction("Error: No token found for username field.");
    }
    len = strlen(token);
    if (len > SIZE_USERNAME)
    {
        errFunction("Error: Username length in file exceeds allowed size.");
    }
    strncpy(buffUsername, token, len);
    buffUsername[len] = '\0';

    strtok(NULL, "\"");
    token = strtok(NULL, "\"");
    if (token == NULL)
    {
        errFunction("Error: No token found for password field.");
    }
    len = strlen(token);
    if (len > SIZE_PASSWORD)
    {
        errFunction("Error: Password length in file exceeds allowed size.");
    }
    strncpy(buffPassword, token, len);
    buffPassword[len] = '\0';
}

// This function compares the message ID from the file to the one given by the user.
// It returns 0 if the IDs are equal, otherwise returns -1.
int static equalIdMessage(char *buffer, char *idMessage)
{
    char *token = strtok(buffer, ",");
    char *pen = NULL;
    char *last = NULL;

    while (token)
    {
        pen = last;
        last = token;
        token = strtok(NULL, ",");
    }

    removeQuotes(pen);

    if (strcmp(idMessage, pen) == 0)
    {
        return 0;
    }
    return -1;
}

// This function sets the presence flag to zero for the given file position.
void static zeroPresence(FILE *f, int pos)
{
    if (fseek(f, pos, SEEK_SET) != 0)
    {
        perror("Error: Failed to seek file position");
        fclose(f);
    }

    if (fputc('0', f) == EOF)
    {
        perror("Error: Failed to write zero presence flag");
        fclose(f);
    }
    fseek(f, pos + 2, SEEK_SET);
}

// This function deletes a message from the file by setting its presence flag to zero.
void delMessageFile(char *file, char *idMessage)
{
    int pos;
    char buffer[SIZE];
    FILE *f = fopen(file, "r+");
    if (f == NULL)
    {
        errFunction("Error: Failed to open file.");
    }

    while (fgets(buffer, sizeof(buffer), f))
    {
        if (equalIdMessage(buffer, idMessage) == 0)
        {
            pos = ftell(f) - 2;
            zeroPresence(f, pos);
        }
    }
    fclose(f);
}

// This function writes a user's details to the user file.
void writeUserFile(char *buffUser, char *buffPassword, char *file)
{
    char buff[SIZE];

    FILE *myFile = fopen(file, "a");
    if (myFile == NULL)
    {
        errFunction("Error: Failed to open file.");
    }

    sprintf(buff, "\"%s\",\"%s\"\n", buffUser, buffPassword);
    fprintf(myFile, "%s", buff);
    fclose(myFile);
}

// This function writes a message to the message file.
void writeMessageFile(char *buffUser, char *buffObj, char *buffText, char *buffIdMessage, char *file)
{
    char buff[SIZE];

    FILE *myFile = fopen(file, "a");
    if (myFile == NULL)
    {
        errFunction("Error: Failed to open file.");
    }

    sprintf(buff, "\"%s\",\"%s\",\"%s\",\"%s\",1\n", buffObj, buffText, buffUser, buffIdMessage);
    fprintf(myFile, "%s", buff);
    fclose(myFile);
}

// This function copies the content of one file to another.
void copyFile(const char *fileSource, const char *fileDestination)
{
    FILE *source, *destination;
    char buffer[1024];
    int bytesRead;

    source = fopen(fileSource, "r");
    if (source == NULL)
    {
        errFunction("Error: Failed to open source file.");
    }

    destination = fopen(fileDestination, "w");
    if (destination == NULL)
    {
        fclose(source);
        errFunction("Error: Failed to open destination file.");
    }

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0)
    {
        fwrite(buffer, 1, bytesRead, destination);
    }

    fclose(source);
    fclose(destination);
}

// This function checks whether the presence flag is zero in a message.
int static checkZeroPresence(char *buffer)
{
    char *pt = strdup(buffer);
    char *token = strtok(pt, ",");
    char *last = NULL;

    while (token)
    {
        last = token;
        token = strtok(NULL, ",");
    }

    if (*last == '0')
    {
        return 0;
    }

    free(pt);
    pt = NULL;
    return -1;
}

// This function eliminates messages with zero presence flag and writes the remaining messages to a shadow file.
void eliminateZeroPresence(char *file, char *fileShadow)
{
    char buffer[SIZE];

    FILE *f = fopen(file, "r+");
    if (f == NULL)
    {
        errFunction("Error: Failed to open file.");
    }

    FILE *newf = fopen(fileShadow, "w+");
    if (newf == NULL)
    {
        errFunction("Error: Failed to open shadow file.");
    }

    rewind(f);
    while (fgets(buffer, SIZE, f))
    {
        if (checkZeroPresence(buffer) != 0)
        {
            fprintf(newf, "%s", buffer);
        }
    }
    fflush(newf);
    fclose(newf);
}


