output: linked_list.o xarpd.o ../my_interface.h communication.o ../definitions.h
	gcc -Wall -pedantic -std=c99 linked_list.o xarpd.o communication.o -o xarpd -pthread

linked_list.o: linked_list.c linked_list.h
	gcc -c linked_list.c

xarpd.o: xarpd.c
	gcc -c xarpd.c

communication.o: ../communication.c ../communication.h
	gcc -c ../communication.c

clean:
	rm *.o xarpd
