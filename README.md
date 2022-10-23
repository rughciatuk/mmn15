# mmn15

The porject is created from 2 parts.

## Client:
Written in C++ Files (Both .h and .cpp):
- AESWrapper - Part of the given libary.
- Base64Wrapper - Part of the given libary (with a small change).
- client - The main logic of the clinet. implement Client class that handle all of the client functions.
- crc - Copied from the internet, claculate the crc of a file.
- main - Contine the main function, that connect to the server and create the client socket. From there the client does the rest.
- protocol - The protocol definetions, both response and request structs and class.
- RSAWrapper - Part of the given libary.

## Server:
Writter in python:
- connection - Handle each connection to the server (Subclass of thread)
- crc - Copied from the internet, calculate the crc of a file.
- db - The logic to talk to the db.
- main - entrypoint, also startup the server.
- protocol - The protocol definetions, both response and request dataclasses.
- server - The server class, listen to new connection and create the new Connection class for each one.