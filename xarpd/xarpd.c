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
#define MAX_PACKET_SIZE 65536
#define MIN_PACKET_SIZE 64

#define MAX_IFACES	64
#define ETH_ADDR_LEN	6

MyInterface my_ifaces[MAX_IFACES];

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

	if(htons(0x0806) == eth->ether_type) {
    unsigned char* arpPacket = (packet + 14);
    struct arp_hdr *arp = (struct arp_hdr*) arpPacket;
    if (ntohs(arp->opcode) == 1)
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
void read_iface(MyInterface *ifn)
{
  struct iface * ifn;
  ifn = (struct iface *) arg;
	socklen_t	saddr_len;
	struct sockaddr	saddr;
	unsigned char	*packet_buffer;
	int		n;

	saddr_len = sizeof(saddr);
	packet_buffer = malloc(MAX_PACKET_SIZE);
	if (!packet_buffer) {
		printf("\nCould not allocate a packet buffer\n");
		exit(1);
	}

	while(1) {
		n = recvfrom(ifn->sockfd, packet_buffer, MAX_PACKET_SIZE, 0, &saddr, &saddr_len);
		if(n < 0) {
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(1);
		}
		doProcess(packet_buffer, n);
	}
}
/* */

// main function
int main(int argc, char** argv)
{
	int i, sockfd;

	if (argc < 2)
		print_usage();

  pthread_t tid[argc - 1];

	for (i = 1; i < argc; i++)
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
	}

	for (i = 0; i < argc-1; i++)
	{
		print_eth_address(my_ifaces[i].name, my_ifaces[i].macAddress);
		printf("\n");
    err = pthread_create(&(tid[i]), NULL, &read_iface, (void *) &my_ifaces[i]);
		// Create one thread for each interface. Each thread should run the function read_iface.
	}
  for (i = 0; i < argc - 1; i++)
  {
    pthread_join(tid[i], NULL);
  }
	return 0;
}
/* */
