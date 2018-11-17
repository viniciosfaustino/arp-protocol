#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>

#include "../my_interface.h"
#include "../definitions.h"
#include "protocol_headers.h"
#include "../communication.h"

#define MAX_PACKET_SIZE 65536
#define MIN_PACKET_SIZE 64

#define MAX_IFACES	64
#define ETH_ADDR_LEN	6

// Global ugly variables
MyInterface *my_ifaces;
int numIfaces;

Node arpTable;
arpTable->next = NULL;

// Print an Ethernet address
void print_eth_address(char *s, unsigned char *eth_addr)
{
	printf("%s %02X:%02X:%02X:%02X:%02X:%02X", s,
	       eth_addr[0], eth_addr[1], eth_addr[2],
	       eth_addr[3], eth_addr[4], eth_addr[5]);
}

// Bind a socket to an interface
int bind_iface_name(int fd, char *iface_name)
{
	return setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, iface_name, strlen(iface_name));
}

void get_iface_info(int sockfd, char *ifname, MyInterface *ifn)
{
	struct ifreq s;

	strcpy(s.ifr_name, ifname);
	if (0 == ioctl(sockfd, SIOCGIFHWADDR, &s))
	{
		memcpy(ifn->macAddress, s.ifr_addr.sa_data, ETH_ADDR_LEN);
		ifn->sockfd = sockfd;
		strcpy(ifn->name, ifname);
	}
	else
	{
		perror("Error getting MAC address");
		exit(1);
	}
}

// Print the expected command line for the program
void print_usage()
{
	printf("\nxarpd <interface> [<interfaces>]\n");
	exit(1);
}


// Break this function to implement the ARP functionalities.
void doProcess(unsigned char* packet, int len) {
	if(!len || len < MIN_PACKET_SIZE)
		return;

	struct ether_hdr* eth = (struct ether_hdr*) packet;

	if(htons(0x0806) == eth->ether_type) // is a arp packet
	{
    unsigned char* arpPacket = (packet + 14);
    struct arp_hdr *arp = (struct arp_hdr*) arpPacket;
    if (ntohs(arp->arp_op) == 1)
    {
      //search the address in the interface table

    }
    else
    {
      // I don't know what to do yet
    }
		// ARP
		//...
	}
	// Ignore if it is not an ARP packet
}


// This function should be one thread for each interface.
void read_iface(MyInterface *arg)
{
  MyInterface *ifn;
  ifn = (MyInterface*) arg;
	socklen_t	saddr_len;
	struct sockaddr	saddr;
	unsigned char	*packet_buffer;
	int n;

	saddr_len = sizeof(saddr);
	packet_buffer = malloc(MAX_PACKET_SIZE);

	if (!packet_buffer)
	{
		printf("\nCould not allocate a packet buffer\n");
		exit(1);
	}

	while(1)
	{
		n = recvfrom(ifn->sockfd, packet_buffer, MAX_PACKET_SIZE, 0, &saddr, &saddr_len);
		if(n < 0) {
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(1);
		}
		doProcess(packet_buffer, n);
	}
}
/* */

void loadIfces(int argc, char **argv)
{
	int sockfd;

	for (int i = 1; i < argc; i++)
	{
		sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
		if(sockfd < 0)
		{
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(1);
		}

		if (bind_iface_name(sockfd, argv[i]) < 0)
		{
			perror("Server-setsockopt() error for SO_BINDTODEVICE");
			printf("%s\n", strerror(errno));
			close(sockfd);
			exit(1);
		}
		get_iface_info(sockfd, argv[i], &my_ifaces[i-1]);
		my_ifaces[i-1].mtu = 1500;
	}
}

void sendIfaces(int socket)
{
	_send(socket, (char*) my_ifaces, numIfaces * sizeof(MyInterface));
}

void setTTL(short int ttl)
{
  //do something
}

void server()
{
	unsigned char BUFFERSIZE = 255;
	char buffer[BUFFERSIZE];

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
  loadSocketInfo(&serv_addr, "0.0.0.0", XARPD_PORT);
  int sockfd = _socket(AF_INET, SOCK_STREAM, 0);
	_bind(&sockfd, (struct sockaddr*) &serv_addr);
	_listen(sockfd, LISTEN_ENQ);
	int n, k, newsockfd;
	char opCode;
	printf("SERVER THREAD IS RUNNING\n");
	while(1)
	{
		printf("READY TO ACCEPT\n");
		newsockfd = _accept(sockfd, (struct sockaddr*) &cli_addr);
		n = 0;
		do
		{
			k = _recv(newsockfd, buffer+n, BUFFERSIZE-n);
			n += k;
		} while(k > 0);
		close(newsockfd);

		if(n > 0)
		{
			opCode = buffer[0];
			printf("OPCODE: %d\n", opCode);
			switch(opCode)
			{
				case LIST_IFCES:
					newsockfd = _accept(sockfd, (struct sockaddr*) &cli_addr);
					sendIfaces(newsockfd);
					printf("IFACES SENT\n");
					break;
        case SET_TTL:
          short int ttl = (buffer[2] << 8) | buffer[1];
          setTTL(ttl);
          printf("TTL SETUP\n");
				default:
					printf("OPERATION NOT SUPPORTED BY XARPD\n");
			}
		}
		close(newsockfd);
	}

}

// main function
int main(int argc, char** argv)
{
	int i;

	if (argc < 2)
		print_usage();

  pthread_t tid[argc];

	numIfaces = argc-1;
	my_ifaces = (MyInterface*) malloc(numIfaces * sizeof(MyInterface));
	memset(my_ifaces, 0, numIfaces * sizeof(MyInterface));

	loadIfces(argc, argv);

	// This thread will be responsible for answer xarp and xifconfig demands
	pthread_create(&tid[argc - 1], NULL, (void*) server, NULL);

	for (i = 0; i < argc-1; i++)
	{
		print_eth_address(my_ifaces[i].name, my_ifaces[i].macAddress);
		printf("\n");
    int err = pthread_create(&(tid[i]), NULL, (void*) read_iface, (void *) &my_ifaces[i]);
		// Create one thread for each interface. Each thread should run the function read_iface.
	}

  for (i = 0; i < argc - 1; i++)
  {
    pthread_join(tid[i], NULL);
  }

	pthread_join(tid[argc-1], NULL);

	// The rest of the code is to respond the requests from xarpd and xifconfig
	return 0;
}
/* */
