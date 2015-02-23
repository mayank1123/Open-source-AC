all:
	g++ AC.cpp -o AC
	g++ WTP.cpp -o WTP
clean:
	rm -f *.o *~ WTP AC

