#include "definitions.h"
#include "xarp.h"
#include "communication.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
  int socket = _socket();
  char requestPacket[13];
  requestPacket[0] = SHOW_TABLE;
  sendPacket(socket, LOOPBACK_IP, XARPD_PORT, requestPacket, 13);

}

char addEntry(const char* ip, const char* mac, const char* ttl)
{
  //do something
}

void setTTL(short int ttl)
{
  //do something
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
