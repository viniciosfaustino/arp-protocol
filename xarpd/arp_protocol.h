#ifndef __ARP_PROTOCOL__
#define __ARP_PROTOCOL__

#include "../my_interface.h"


char* buildArpRequest(unsigned int, unsigned char*, unsigned int);
int sendArpRequest(char*, MyInterface*);

#endif
