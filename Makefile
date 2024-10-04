default: 
	gcc client.c -o cliento
main: 
	gcc helper.c messageLib.c main.c -o prova
	./prova
server: 
	gcc helper.c messageLib.c server.c -o server
	./server
