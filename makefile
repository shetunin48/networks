all:
	gcc server_client.c -o server_client.o -lpcap
clean:
	rm -f *.o