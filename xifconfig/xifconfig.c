#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h> // close function

#include "../my_interface.h"
#include "../communication.h"
#include "../definitions.h"

// void setInterface(const char* ipAddr, const char* ipNetmask, char* name)
// {
//   struct ifreq ifr;
//   int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
//
//   strncpy(ifr.ifr_name, name, IFNAMSIZ);
//
//   ifr.ifr_addr.sa_family = AF_INET;
//
//   inet_pton(AF_INET, ipAddr, ifr.ifr_addr.sa_data + 2);
//   ioctl(fd, SIOCSIFADDR, &ifr);
//
//   inet_pton(AF_INET, ipNetmask, ifr.ifr_addr.sa_data + 2);
//   ioctl(fd, SIOCSIFNETMASK, &ifr);
//
// }
//
// void setMTUSize(int mtu, const char* name)
// {
//   struct ifreq ifr;
//   strncpy(ifr.ifr_name, name, IFNAMSIZ);
//   ifr.ifr_mtu = mtu;
//   int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
//   ioctl(fd, SIOCSIFMTU, &ifr);
// }

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
  printf("UP MTU: %d\n", interface->mtu);
  printf("RX packets: %d\t", interface->rxPackets);
  printf("TX packets: %d\n", interface->txPackets);
  printf("RX bytes: %ld\t", interface->rxBytes);
  printf("TX bytes: %ld\n", interface->rxBytes);

}

void makeNewSocketAndConnect(int *socket, struct sockaddr_in* serv_addr)
{
  *socket = _socket(AF_INET, SOCK_STREAM, 0);
  _connect(*socket, serv_addr, sizeof(*serv_addr));
}

void listIfaces(struct sockaddr_in* serv_addr)
{
  int socket;
  makeNewSocketAndConnect(&socket, serv_addr);

  char request = LIST_IFCES;
  _send(socket, &request, 1);
  close(socket);

  int interfaceLength = sizeof(MyInterface);
  char *buffer = (char*) malloc(interfaceLength);
  int n = 0;
  makeNewSocketAndConnect(&socket, serv_addr);
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

int main(int argc, char const *argv[])
{
  // Builds the essential to communicate with xarpd
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  // Now sockfd is read and can be used to communicate with xarpd
  if (argc == 1)
  {
    listIfaces(&serv_addr);
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
    // setInterface(argv[2], argv[3], argv[1]);
  }
  return 0;
}
