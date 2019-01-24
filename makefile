all: TCP_multithread_server.o TCP_multithread_client.o

TCP_multithread_client.o: TCP_client.c	
	gcc TCP_client.c -o TCP_multithread_client.o

TCP_multithread_server.o: TCP_multithread_server.c
	gcc TCP_multithread_server.c -o TCP_multithread_server.o