all:
	g++ AC.cpp -o AC
	g++ client.cpp -o client
clean:
	rm -f *.o *~ client server AC

