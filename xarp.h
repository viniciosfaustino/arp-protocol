#ifndef __XARP__
#define __XARP__

char getOperation(const char*);

void showArpTable(void);

void resolveAddress(const char *);

char addEntry(const char*, const char*, const char*);

char delEntry(const char *);

void setTTL(short int);

#endif
