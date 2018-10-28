#ifndef __LINKED_LIST__

#define __LINKED_LIST__


// Node of the linked list where the ARP table will be stored
typedef struct lNode
{
  unsigned int id;
  unsigned int ipAddress;
  unsigned int macAddress[6];
  short int ttl;
  struct lNode *next;
} Node;

char addLine(Node*, Node*);

char removeLine(Node*, unsigned int);

char searchLine(Node*, unsigned int, Node*);

void printLine(Node*);

void printTable(Node*);

#endif
