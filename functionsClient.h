#define SIZE 1024

//eh qui sta un problema dovuto al fatto che ho message che intende due cose
//ma è uguale o almeno sta uno a uno al CODE_MESSAGE
#define SIZE_NUM_MSG 16


#define SIZE_USERNAME 64
#define SIZE_PASSWORD 64

#define SIZE_AUTH_MESSAGE (SIZE_USERNAME + SIZE_PASSWORD )

#define SIZE_OBJECT 64
#define SIZE_TEXT 160

#define SIZE_MESSAGE_ID 6

#define SIZE_MSG_MESSAGE (SIZE_OBJECT + SIZE_TEXT)

#define SIZE_PERSONAL_COMPLETE_MESSAGE ( SIZE_OBJECT + SIZE_TEXT + SIZE_MESSAGE_ID)
#define SIZE_GENERIC_COMPLETE_MESSAGE ( SIZE_USERNAME + SIZE_OBJECT + SIZE_TEXT + SIZE_MESSAGE_ID)



#define COMMAND_AUTH   0x01
#define COMMAND_LOG    0x02
#define COMMAND_SUB    0x03

//sostituire il fail in alcuni casa con resend data 
//i tenativi di quanti farli e meglio averli nel server
#define COMMAND_ERR_USER_NOT_FOUND    0x04
#define COMMAND_ERR_NOT_MATCH_CREDENTIALS     0x05
#define COMMAND_ERR_USER_ALREADY_EXISTS 0x06
#define COMMAND_SUCCESS    0x07
#define COMMAND_FAILURE 0x08
#define COMMAND_CLOSE 0x09


#define COMMAND_POST_MSG 0x10
#define COMMAND_VIEW_MSG 0x11
#define COMMAND_VIEW_ALL_MSG 0x12
#define COMMAND_DELETE_MSG 0x13
#define COMMAND_QUIT 0x14

#define COMMAND_FINISH_ATTEMPTS 0x15

void clientFunc(int socket); 

