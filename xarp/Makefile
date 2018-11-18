output: xarp.h xarp.c communication.o ../definitions.h
	gcc -Wall -pedantic -std=c99 communication.o xarp.c -o xarp

communication.o: ../communication.h ../communication.c
	gcc -c -Wall -pedantic -std=c99 ../communication.c

clean:
	rm *.o xarp
