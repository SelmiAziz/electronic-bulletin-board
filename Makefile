client: 
	gcc src/helper.c src/protocolUtilis.c src/functionsClient.c src/client.c -o myClient
	./myClient a 127.0.0.1 p 2500
server:
	gcc src/helper.c src/protocolUtilis.c src/fileMessageLib.c src/messageLib.c src/functionServer.c src/server.c -o myServer
	./myServer
