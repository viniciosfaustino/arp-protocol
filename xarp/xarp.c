#include "../definitions.h"
#include "xarp.h"
#include "../communication.h"
#include "../linked_list.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close function

char getOperation(const char* c)
{
  if(strcmp("show", c) == 0) return SHOW_TABLE;
  else if(strcmp("res", c) == 0) return RES_IP;
  else if(strcmp("add", c) == 0) return ADD_LINE;
  else if(strcmp("del", c) == 0) return DEL_LINE;
  else if(strcmp("ttl", c) == 0) return SET_TTL;

  return __ERROR__;
}

void showArpTable()
{
  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);

  char request = SHOW_TABLE;
  _send(socket, &request, 1);
  close(socket);

  int lineLen = sizeof(Node);
  char *buffer = (char*) malloc(lineLen);
  int n = 0;
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);
  int count = 0;
  do
  {
    if(n == lineLen) n = 0; // clean n for next interation

    n += _recv(socket, buffer+n, lineLen-n);
    if(n == lineLen)
    {
      // printInterface((MyInterface*) buffer);
      printLine((Node*) buffer, count++);
      printf("\n");
    }
  } while(n);

  free(buffer);
  close(socket);

}

char addEntry(const char* ipAddr, const char* macAddress, const char* ttl)
{
  unsigned int ip = inet_addr(ipAddr); // converts from dot notation into binary
  short int ttlSize = atoi(ttl);
  char mac[6];
  sscanf(macAddress, "%x:%x:%x:%x:%x:%x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

  // Prepares info to send
  // opcode ifacenName ipAddress netmask
  unsigned char messageLen = 13;
  char message[messageLen];
  message[0] = ADD_LINE;
  memcpy(message+1, (char*)&ip, 4);
  memcpy(message+1+4, (char*)&mac, 6);
  memcpy(message+1+4+6, (char*)&ttlSize, 2);

  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);

  _send(socket, message, messageLen);
  close(socket);

  return __OK__;
}

void setTTL(short int ttl)
{
  //do something
}

int buildCommunicationWithXARP()
{
  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);
  return socket;
}

char delEntry(const char *ipAddress)
{
  // Opcode ipAddress
  unsigned char messageLen = 1 + 4;
char message[messageLen];

  message[0] = DEL_LINE;

  unsigned int ip = inet_addr(ipAddress);
  memcpy(message+1, (char*) &ip, 4);

  int socket = buildCommunicationWithXARP();
  _send(socket, message, messageLen);
  close(socket);

  return __OK__;
}

void resolveAddress(const char *ipAddress)
{

}

// In the main will be implemented the parser
int main(int argc, char *argv[])
{
  // Naive check
  if(argc < 2 || argc > 5)
  {
    // print error
    exit(1);
  }

  char opCode = getOperation(argv[1]);
  char ret;
  switch(opCode)
  {
    case SHOW_TABLE:
      if(argc != 2)
      {
        // print error
        exit(1);
      }
      showArpTable();
      break;
    case RES_IP:
      if(argc != 3)
      {
        // print error
        exit(1);
      }
      resolveAddress(argv[2]);
      break;
    case ADD_LINE:
      if(argc == 5)
      {
        // print error
        exit(1);
      }
      ret = addEntry(argv[2], argv[3], argv[4]);
      if(ret == __OK__) printf("Input added correctly\n");
      else printf("Error when adding entry\n");
      break;
    case DEL_LINE:
      if(argc != 3)
      {
        // print error
        exit(1);
      }
      ret = delEntry(argv[2]);
      if(ret == __OK__) printf("Entry deleted sucessfully\n");
      else printf("There is no entry with this address");
      break;
    case SET_TTL:
      if(argc != 3)
      {
        // print error
        exit(1);
      }
      setTTL((short int) atoi(argv[2]));
      break;

    default:
      // print error
      exit(1);
  }
}
