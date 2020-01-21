#include "id.h"
#include "utils.h"
#include "version.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_PORT 81
#define AGENT_TYPE "linux-agent:" VERSION

int main(int argc, char const *argv[]) {
  signal(SIGPIPE, SIG_IGN);

  printf("starting linux-agent %s\n", VERSION);

  int i = 0;
  int n;

  struct sockaddr_in serverAddress;
  int serverAddrLen = sizeof(serverAddress);
  int serverFd;

  startHttpSrv(&serverAddress, &serverFd, SERVER_PORT);

  while (1) {
    int clientFd;

    printf("waiting for client connection\n");
    if ((clientFd = accept(serverFd, (struct sockaddr *)&serverAddress,
                           (socklen_t *)&serverAddrLen)) < 0) {
      perror("client connection accept");
      exit(EXIT_FAILURE);
    }

    send(clientFd, AGENT_TYPE, strlen(AGENT_TYPE), 0);
    close(clientFd);
  }

  return 0;
}
