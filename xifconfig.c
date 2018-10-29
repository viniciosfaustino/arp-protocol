#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <sys/socket.h>
#include "myInterface.h"

void setInterface(int ipAddr, int ipNetmask, char* name)
{
  struct ifreq ifr;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

  struct sockaddr_in addr;
  addr.sin_addr.s_addr = ipAddr;


  strncpy(ifr.ifr_name, name, IFNAMSIZ);

  ifr.ifr_addr.sa_family = AF_INET;
  char *ipAd = inet_ntoa(addr.sin_addr);
  addr.sin_addr.s_addr = ipNetmask;
  char *ipNM = inet_ntoa(addr.sin_addr);

  inet_pton(AF_INET, ipAd, ifr.ifr_addr.sa_data + 2);
  ioctl(fd, SIOCSIFADDR, &ifr);

  inet_pton(AF_INET, ipNM, ifr.ifr_addr.sa_data + 2);
  ioctl(fd, SIOCSIFNETMASK, &ifr);

}

void setMTUSize(int mtu, char* name)
{
  struct ifreq ifr;
  strncpy(ifr.ifr_name, name, IFNAMSIZ);
  ifr.ifr_mtu = mtu;
  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  ioctl(fd, SIOCSIFMTU, &ifr);
}

void printInterface(MyInterface interface )
{
  struct sockaddr_in addr;
  addr.sin_addr.s_addr = interface.ipAddress;
  printf("%s\t", interface.name);
  printf("Endereco de HW %2X:%2X:%2X:%2X:%2X:%2X \n", interface.macAddress[0], interface.macAddress[1], interface.macAddress[2], interface.macAddress[3], interface.macAddress[4], interface.macAddress[5]);
  printf("inet end.: %s\t",inet_ntoa(addr.sin_addr));
  addr.sin_addr.s_addr = interface.broadcastAddress;
  printf("Bcast: %s\t",inet_ntoa(addr.sin_addr));
  addr.sin_addr.s_addr = interface.netMask;
  printf("Masc: %s\n",inet_ntoa(addr.sin_addr));
  printf("UP MTU: %d\n", interface.mtu);
  printf("RX packets: %d\t", interface.rxPackets);
  printf("TX packets: %d\n", interface.txPackets);
  printf("RX bytes: %ld\t", interface.rxBytes);
  printf("TX bytes: %ld\n", interface.rxBytes);

}
int main(int argc, char const *argv[]) {
  /* code */
  return 0;
}
