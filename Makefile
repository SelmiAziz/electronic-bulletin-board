CC = gcc
CFLAGS = -Wall
CLIENT = myClient
SERVER = myServer

# Default host and port
DEFAULT_HOST = 127.0.0.1
DEFAULT_PORT = 2500


HOST ?= $(DEFAULT_HOST)
PORT ?= $(DEFAULT_PORT)

# Compile the client
client:
	$(CC) src/helper.c src/protocolUtilis.c src/utilityForFunctionClient.c src/functionClient.c src/client.c -o $(CLIENT)

# Compile the server
server:
	$(CC) src/helper.c src/protocolUtilis.c src/fileMessageLib.c src/messageLib.c src/utilityForFunctionServer.c src/mutexLib.c src/serverConfig.c src/functionServer.c  src/server.c -o $(SERVER)

# Run the client with parameters passed through the command line
run-client:
	./$(CLIENT) a $(HOST) p $(PORT)

# Run the client with default values (127.0.0.1, 2500)
run-client-default:
	./$(CLIENT) a $(DEFAULT_HOST) p $(DEFAULT_PORT)

# Run the server
run-server:
	./$(SERVER)


