#include"messageLib.h"



int main(int arcv, char *argv)
{
	BulletinBoard *myBoard = createBulletinBoard(); 
	addUser(myBoard,"mario", "Marione45");
	addMessageUser(myBoard, "mario", "balotelli", "federicoo", "back_messages.csv"); 
	printUser(myBoard->head); 


}
