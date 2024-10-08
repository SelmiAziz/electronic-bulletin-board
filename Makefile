client: 
	gcc helper.c functionsClient.c client.c -o myClient
	./myClient
server:
	gcc helper.c messageLib.c functionServer.c server.c -o myServer
	./myServer
