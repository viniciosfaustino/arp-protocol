#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h> // close function
#include <netinet/in.h>
#include <string.h>

#include "../my_interface.h"
#include "../communication.h"
#include "../definitions.h"
#include "xifconfig.h"


void printInterface(MyInterface *interface)
{
  struct sockaddr_in addr;
  addr.sin_addr.s_addr = interface->ipAddress;
  printf("%s\t", interface->name);
  printf("Endereco de HW %2X:%2X:%2X:%2X:%2X:%2X \n", interface->macAddress[0], interface->macAddress[1], interface->macAddress[2], interface->macAddress[3], interface->macAddress[4], interface->macAddress[5]);
  printf("inet end.: %s\t",inet_ntoa(addr.sin_addr));
  addr.sin_addr.s_addr = interface->broadcastAddress;
  printf("Bcast: %s\t",inet_ntoa(addr.sin_addr));
  addr.sin_addr.s_addr = interface->netMask;
  printf("Masc: %s\n",inet_ntoa(addr.sin_addr));
  printf("UP MTU: %d\n", ntohs(interface->mtu));
  printf("RX packets: %d\t", ntohl(interface->rxPackets));
  printf("TX packets: %d\n", ntohl(interface->txPackets));

  int *rx = (int*) &(interface->rxBytes);
  rx[0] = ntohl(rx[0]);
  rx[1] = ntohl(rx[1]);
  printf("RX bytes: %ld\t", *(unsigned long*)rx);
  int *tx = (int*) &(interface->txBytes);
  tx[0] = ntohl(tx[0]);
  tx[1] = ntohl(tx[1]);
  printf("TX bytes: %ld\n", *(unsigned long*)tx);
}

void listIfaces()
{
  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);

  char request = LIST_IFCES;
  _send(socket, &request, 1);
  close(socket);

  int interfaceLength = sizeof(MyInterface);
  char *buffer = (char*) malloc(interfaceLength);
  int n = 0;
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);
  do
  {
    if(n == interfaceLength) n = 0; // clean n for next interation

    n += _recv(socket, buffer+n, interfaceLength-n);
    if(n == interfaceLength)
    {
      printInterface((MyInterface*) buffer);
      printf("\n");
    }
  } while(n);

  free(buffer);
  close(socket);
}

void configIface(const char *ifaceName, const char *ipAddress, const char *netmask)
{
  unsigned int ip = inet_addr(ipAddress); // converts from dot notation into binary
  unsigned int mask = inet_addr(netmask); // in network byte order!
  unsigned char ifaceNameLen = strlen(ifaceName);

  // Prepares info to send
  // opcode ifacenName ipAddress netmask
  unsigned char messageLen = 1 + ifaceNameLen+1 + 4 + 4;
  char message[messageLen];
  message[0] = CONFIG_IFACE;
  memcpy(message+1, ifaceName, ifaceNameLen+1);
  memcpy(message+1+ifaceNameLen+1, (char*)&ip, 4);
  memcpy(message+1+ifaceNameLen+1+4, (char*)&mask, 4);

  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);

  _send(socket, message, messageLen);
  close(socket);
}

int main(int argc, char const *argv[])
{
  if (argc == 1)
  {
    listIfaces();
  }
  else if(strcmp(argv[2], "mtu") == 0)
  {
    if (argc == 4)
    {
      int size = atoi(argv[3]);
      // setMTUSize(size, argv[1]);
    }else
    {
      // setMTUSize(1500, argv[1]);
    }
  }
  else
  {
    configIface(argv[1], argv[2], argv[3]);
  }
  return 0;
}
