#ifndef __XIFCONFIG__
#define __XIFCONFIG__

#include <netinet/in.h>
#include "../my_interface.h"

void setInterface(int, int);

void setMTUSize(int);

void printInterface(MyInterface*);

void listIfaces();

void configIface(const char*, const char*, const char*);

#endif
