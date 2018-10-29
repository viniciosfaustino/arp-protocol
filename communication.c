#include "communication.h"

#include <stdlib.h>


void errorHandler(void)
{
  fprintf(stderr, "ERROR: %s\n", strerror(errno));
  exit(1);
}

int _socket(void)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    errorHandler();
  }
  return sockfd;
}

void loadSocketInfo(struct sockaddr_in *serv_addr, int port)
{
  memset((char*) serv_addr, 0, sizeof(*serv_addr));
  serv_addr->sin_family = AF_INET;
  serv_addr->sin_addr.s_addr = INADDR_ANY;
  serv_addr->sin_port = port;
}


void _bind(int *sockfd, struct sockaddr *address)
{
  if(bind(*sockfd, address, sizeof(*address)) < 0)
  {
    errorHandler();
  }
}

void _listen(int sockfd, unsigned int queueSize)
{
  if(listen(sockfd, queueSize) < 0)
  {
    errorHandler();
  }
}


int _accept(int sockfd, struct sockaddr *cli_addr)
{
  unsigned int lenCliAddr = sizeof(*cli_addr);
  int newsockfd = accept(sockfd, cli_addr, &lenCliAddr);
  if(newsockfd < 0)
  {
    printf("lenCliAddr: %d\n",lenCliAddr);
    errorHandler();
  }

  return newsockfd;
}

int _recv(int socket, char *buffer, unsigned int bufferSize)
{
  memset(buffer, 0, bufferSize);

  int n = recv(socket, buffer, bufferSize, 0);
  if(n < 0)
  {
    errorHandler();
  }
  return n;
}

void _send(int newsockfd, char *message, unsigned int messageSize)
{
  int n = 0;
  do
  {
      n += send(newsockfd, message+n, messageSize - n, 0);
      if (n < 0)
      {
        errorHandler();
      }
  } while (n < messageSize);
}
