# Makefile for the socket programming example
#

#simple_server_objects = ServerSocket.o Socket.o simple_server_main.o
GUISocketTest = ClientSocket.o ../Socket.o GUISocketTest.o


all : GUITest

#simple_server: $(simple_server_objects)
#	g++ -o simple_server $(simple_server_objects)


GUITest: $(simple_client_objects)
	g++ -o GUITest $(simple_client_objects)


Socket: ../Socket.cpp
#ServerSocket: ../ServerSocket.cpp
ClientSocket: ClientSocket.cpp
#simple_server_main: simple_server_main.cpp
GUISocketTest: GUISocketTest.cpp


clean:
	rm -f *.o simple_client
