#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>
#include <sys/socket.h>

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

void printInterface()
{

}
int main(int argc, char const *argv[]) {
  /* code */
  return 0;
}
