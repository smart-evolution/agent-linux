#include "id.h"
#include "utils.h"
#include "version.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERVER_PORT 81
#define AGENT_TYPE "linux-agent:" VERSION

typedef struct pth_params {
  int serverFd;
  struct sockaddr_in serverAddress;
} pth_params;

void *tcpLoop(void *pthread_args) {
  pth_params *args = (pth_params *)pthread_args;

  int serverAddrLen = sizeof(args->serverAddress);

  while (1) {
    int clientFd;

    printf("waiting for client connection\n");
    if ((clientFd =
             accept(args->serverFd, (struct sockaddr *)&args->serverAddress,
                    (socklen_t *)&serverAddrLen)) < 0) {
      perror("client connection accept");
      exit(EXIT_FAILURE);
    }

    send(clientFd, AGENT_TYPE, strlen(AGENT_TYPE), 0);
    close(clientFd);
  }
}

void *tcpLoop(void *pthread_args) {}

int main(int argc, char const *argv[]) {
  signal(SIGPIPE, SIG_IGN);

  printf("starting linux-agent %s\n", VERSION);

  int i = 0;
  int n;

  struct sockaddr_in serverAddress;
  int serverFd;

  startTCPSrv(&serverAddress, &serverFd, SERVER_PORT);

  pthread_t tid;
  pth_params params;
  params.serverFd = serverFd;
  params.serverAddress = serverAddress;

  pthread_create(&tid, NULL, tcpLoop, (void *)&params);
  pthread_exit(NULL);

  return 0;
}
