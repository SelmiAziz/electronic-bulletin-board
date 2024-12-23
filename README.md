# Electronic Bulletin Board

This application is a bulletin board system that allows users to post, view, and manage their messages. It includes a client and a server where the client interacts with the server to perform actions on the board.

## Client Features

The client can do the following actions:

- Insert a message.
- View messages they have posted.
- View all messages.
- Delete a message they have posted.

## How to Compile and Run

Once inside the main folder: electronic-bulletin-board.

###Server

To compile the server use the following command : make server

To run the server use the following command : make run-server

###Client

To compile the client use the following command : make client

To run the client use the following command : make run-client

By default, the client will connect to 127.0.0.1 (localhost) on port 2500. If you want to connect to a different server, you can specify the host and port like this: make run-client HOST=server.address PORT=PortNumber


