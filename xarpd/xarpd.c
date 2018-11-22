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
#include <semaphore.h>
#include <time.h> // used to build the timeout argument for sem_timedwait

#include "../my_interface.h"
#include "../definitions.h"
#include "protocol_headers.h"
#include "../communication.h"
#include "../linked_list.h"
#include "arp_protocol.h"

#define MAX_PACKET_SIZE 65536
#define MIN_PACKET_SIZE 64

#define MAX_IFACES	64
#define ETH_ADDR_LEN	6

// Global ugly variables

// interfaces things
MyInterface *my_ifaces;
sem_t *ifaceMutexes;
int numIfaces;

// arp table
Node arpTable;
short int currentTTL = _DEFAULT_ARP_TTL_;

// mutex for server thread used in xarp res
sem_t serverSemaphore;

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
void doProcess(unsigned char* packet, int len, MyInterface *iface)
{
	if(!len || len < MIN_PACKET_SIZE)
		return;

	struct ether_hdr* eth = (struct ether_hdr*) packet;

	if(htons(0x0806) == eth->ether_type) // is a arp packet
	{
    unsigned char* arpPacket = (packet + 14);
    struct arp_hdr *arp = (struct arp_hdr*) arpPacket;
		unsigned short type;
		type = ntohs(arp->arp_op);
    if (type == ARP_REQUEST)
    {
			// Sees if this iface has the destination ip of the arp packet
      if(ntohl(arp->arp_dpa) == iface->ipAddress)
			{
				char *reply = buildArpReply(iface->ipAddress, iface->macAddress, ntohl(arp->arp_spa), arp->arp_sha);
				sendArpPacket(reply, iface);
				free(reply);
			}
    }
    else
    {
			if(type == ARP_REPLY)
			{
					// adds to list and wakes server thread
			}
    }
		// ARP
		//...
	}
	// Ignore if it is not an ARP packet
}


// This function should be one thread for each interface.
void read_iface(void *arg)
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
		doProcess(packet_buffer, n, ifn);
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

void iface2NetworkByteOrder(MyInterface *iface)
{
	iface->sockfd = htonl(iface->sockfd);
	iface->ttl = htons(iface->ttl);
  iface->mtu = htons(iface->mtu);
  iface->ipAddress = htonl(iface->ipAddress);
  iface->broadcastAddress = htonl(iface->broadcastAddress);
  iface->netMask = htonl(iface->netMask);
  iface->rxPackets = htonl(iface->rxPackets);
  iface->txPackets = htonl(iface->txPackets);

	int *rx = (int*) &(iface->rxBytes);
	rx[0] = htonl(rx[0]);
	rx[1] = htonl(rx[1]);

	int *tx = (int*) &(iface->txBytes);
	tx[0] = htonl(tx[0]);
	tx[1] = htonl(tx[1]);
}

void line2NetworkByteOrder(Node *line)
{
  line->ipAddress = htonl(line->ipAddress);
  line->ttl = htons(line->ttl);
}

void sendIfaces(int socket)
{
	MyInterface aux;
	unsigned int myIfaceLen = sizeof(MyInterface);
	for(int i = 0; i < numIfaces; i++)
	{
		aux = my_ifaces[i]; // a shallow copy is enough
		//converts ifaces atributes to network byte order
		iface2NetworkByteOrder(&aux);
		_send(socket, (char*) &aux, myIfaceLen);
	}

}

void sendLines(int socket)
{
  Node *line = &arpTable;
	Node aux;
  unsigned int lineLen = sizeof(Node);
  while (line->next != NULL)
  {
		aux = *(line->next);
    line2NetworkByteOrder(&aux);
    _send(socket, (char*) &aux, lineLen);
    line = line->next;
  }
}

unsigned char getIfaceIndex(const char *ifname)
{
	unsigned char i;
	for(i = 0; i < numIfaces; i++)
	{
		// paglijonson style
		if(strcmp(my_ifaces[i].name, ifname) == 0) break;
	}
	return i;
}

void configIface(const char *ifname, unsigned int ip, unsigned int mask)
{
	unsigned char i = getIfaceIndex(ifname);

	if(i < numIfaces) // iface found
	{
		sem_wait(&ifaceMutexes[i]);
		my_ifaces[i].ipAddress = ip;
		my_ifaces[i].netMask = mask;
		sem_post(&ifaceMutexes[i]);
	}
}

void setMTUSize(const char *ifname, unsigned short mtu)
{
	unsigned char i = getIfaceIndex(ifname);

	if(i < numIfaces)
	{
		printf("%s: %u\n", ifname, mtu);
		sem_wait(&ifaceMutexes[i]);
		my_ifaces[i].mtu = mtu;
		sem_post(&ifaceMutexes[i]);
	}
}

void setTTL(short int ttl)
{
  //do something
}

void delLine(unsigned int ipAddress)
{
	removeLine(&arpTable, ipAddress);
}

void resolveIP(unsigned int ip, int socket)
{
	// first thing is to look for the ip at the arp table
	Node *line = searchLine(&arpTable, ip);

	if(line == NULL) // line not found
	{
		// searchs the ie in the same network of the requested ip address
		unsigned char i;
		unsigned int ifaceIP, ifaceNetmask;
		unsigned char *ifaceMAC;
		for(i = 0; i < numIfaces; i++)
		{
			sem_wait(&ifaceMutexes[i]);
			ifaceIP = my_ifaces[i].ipAddress;
			ifaceNetmask = my_ifaces[i].netMask;
			ifaceMAC = my_ifaces[i].macAddress;
			sem_post(&ifaceMutexes[i]);

			if((ifaceIP & ifaceNetmask) == (ip & ifaceNetmask)) break;
		}

		if(i < numIfaces) // ie some iface's network matches with ip resquested network
		{
			char *request = buildArpRequest(ifaceIP, ifaceMAC, ip);
			sendArpPacket(request, &my_ifaces[i]);
			free(request);

			// get time info
			clockid_t clockID = CLOCK_REALTIME; // this clock has the time in seconds
																					 // and nanoseconds since THE EPOCH
																					 // what happens when the representation ends ?? POW

			struct timespec ts; //  struct to stores the time
			clock_gettime(clockID, &ts);
			ts.tv_sec += ARP_TIMEOUT;

			// waits for an answer
			// locks the interface for receiving packets
			int ret = sem_timedwait(&serverSemaphore, &ts);

			if(ret != ETIMEDOUT) // ie: timeout was not exceeded
			{
				line = searchLine(&arpTable, ip);
			}
		}
	}
	char response[9];
	if(line == NULL) // ie error
	{
		response[0] = __ERROR__;
	}
	else
	{
		response[0] == __OK__;
		short int ttl = htons(line->next->ttl);
		memcpy(response+1, line->next->macAddress, 6);
		memcpy(response+7, &ttl, 2);
	}
	_send(socket, (char*) &response, 9);
}

void server()
{
	unsigned char BUFFERSIZE = 255;
	char buffer[BUFFERSIZE];

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  int sockfd = _socket(AF_INET, SOCK_STREAM, 0);
	_bind(&sockfd, (struct sockaddr*) &serv_addr);
	_listen(sockfd, LISTEN_ENQ);

	int n, k, newsockfd;
	char opCode;
	char *message; // aux to message decoding
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
			message = buffer + 1;
			char ifName[MAX_IFNAME_LEN];
      unsigned int ip;
			switch(opCode)
			{
				case LIST_IFCES:
					newsockfd = _accept(sockfd, (struct sockaddr*) &cli_addr);
					sendIfaces(newsockfd);
					printf("IFACES SENT\n");
					break;
				case CONFIG_IFACE:
					// message decode
					strcpy(ifName, message);
					unsigned char nameLen = strlen(ifName);
					message += nameLen+1;
					ip = ntohl(*(unsigned int*)message);
					message += 4;
					unsigned int mask = ntohl(*(unsigned int*)message);
					configIface(ifName, ip, mask);
					break;
        case SET_IFACE_MTU:
					// message decode
					strcpy(ifName, message);
					unsigned char ifaceNameLen = strlen(ifName);
					unsigned short mtuSize = ntohs(* (unsigned short*)(message+ifaceNameLen+1));
					setMTUSize(ifName, mtuSize);
					break;

        case ADD_LINE:
          //add a new line on arp table
          ip = ntohl(*(unsigned int*)message);
          message += 4 + 6;
          short int ttl = ntohs(*(short int*)message);

          Node *l = newLine(ip, message - 6, ttl);
          addLine(&arpTable, l, STATIC_ENTRY);
          break;
        case SHOW_TABLE:
          newsockfd = _accept(sockfd, (struct sockaddr*) &cli_addr);
          sendLines(newsockfd);
          break;

				case DEL_LINE:
					ip = ntohl(*(unsigned int*) message);
					delLine(ip);
					break;

				case RES_IP:
					ip = ntohl(*(unsigned int*) message);
					newsockfd = _accept(sockfd, (struct sockaddr*) &cli_addr);
					resolveIP(ip, newsockfd);
					break;

				default:
					printf("OPERATION NOT SUPPORTED BY XARPD\n");
			}
			close(newsockfd);
		}
	}

}

void decrementer()
{
  Node *line;
	Node aux;
  while(1){
    sleep(1);
    line = &arpTable;
    while (line->next != NULL)
    {
      if ((line->next)->ttl-- == 0 || (line->next)->ttl == 0)
      {
        removeLine(&arpTable,(line->next)->ipAddress);
      }
      if (line->next != NULL)
      {
        line = line->next;
      }
    }
  }
}

void initMutexes(int numSem)
{
	ifaceMutexes = (sem_t*) malloc(numSem * sizeof(sem_t));
	for(int i = 0; i < numSem; i++)
	{
		sem_init(&ifaceMutexes[i], 0, 1); // mutex compartilhado por todas as threads
	}
}

// main function
int main(int argc, char** argv)
{
	arpTable.next = NULL;
	sem_init(&(arpTable.semaphore), 0, 1);

	sem_init(&serverSemaphore, 0 , 0);

	int i;
	if (argc < 2)
		print_usage();

  pthread_t tid[argc];
  pthread_t ttlDecrementer;

	numIfaces = argc-1;
	my_ifaces = (MyInterface*) malloc(numIfaces * sizeof(MyInterface));
	memset(my_ifaces, 0, numIfaces * sizeof(MyInterface));

	initMutexes(argc-1);
	loadIfces(argc, argv);

	// This thread will be responsible for answer xarp and xifconfig demands
	pthread_create(&tid[argc - 1], NULL, (void*) server, NULL);

  pthread_create(&ttlDecrementer, NULL, (void*) decrementer, NULL);

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

  pthread_join(ttlDecrementer, NULL);

	// The rest of the code is to respond the requests from xarpd and xifconfig
	return 0;
}
/* */
