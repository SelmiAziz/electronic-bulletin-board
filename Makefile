client: 
	gcc helper.c functionsClient.c client.c -o myClient
	./myClient a 127.0.0.1 p 2500
server:
	gcc helper.c messageLib.c functionServer.c server.c -o myServer
	./myServer
