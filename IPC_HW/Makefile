server : server.c client1 client2 client3
	gcc -o server server.c

client1 : client1.c
	gcc -o client1 client1.c -lpthread

client2 : client2.c
	gcc -o client2 client2.c -lpthread

client3 : client3.c
	gcc -o client3 client3.c -lpthread

clean :
	./delete_ipc
	rm -f *.o server client1 client2 client3
