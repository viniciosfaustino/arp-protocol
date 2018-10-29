#ifndef __MY_INTERFACE__
#define __MY_INTERFACE__

typedef struct
{
  unsigned char name[IFNAMSIZ];
  unsigned char macAddress[6];
  unsigned int ipAddress;
  unsigned int broadcastAddress;
  unsigned int netMask;
  unsigned short mtu;
  unsigned int rxPackets;
  unsigned int txPackets;
  unsigned long int rxBytes;
  unsigned long int txBytes;
} MyInterface;


#endif
