#ifndef __COMMUNICATION__
#define __COMMUNICATION__

void errorHandler(void);

int _socket(void);

void loadSocketInfo(struct sockaddr_in*, int);

void _bind(int*, struct sockaddr*);

void _listen(int, unsigned int);

int _recv(int, char*, unsigned int);

void _send(int, char*, unsigned int);

#endif
