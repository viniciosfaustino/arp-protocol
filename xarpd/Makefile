output: linked_list.o xarpd.o communication.o arp_protocol.o ../definitions.h ../my_interface.h
	gcc -Wall -pedantic -std=c99 linked_list.o xarpd.o communication.o arp_protocol.o -o xarpd -pthread

linked_list.o: ../linked_list.c ../linked_list.h
	gcc -c ../linked_list.c

xarpd.o: xarpd.c
	gcc -c xarpd.c

communication.o: ../communication.c ../communication.h
	gcc -c ../communication.c

arp_protocol.o: arp_protocol.h arp_protocol.c
	gcc -c -Wall -pedantic -std=c99 arp_protocol.c

clean:
	rm *.o xarpd
