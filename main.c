#include "http.h"
#include "id.h"
#include "tcp.h"
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

#define TCP_SERVER_PORT 81
#define HTTP_SERVER_PORT 80
#define AGENT_TYPE "linux-" VERSION

typedef struct pth_params {
  int serverFd;
  struct sockaddr_in serverAddress;
} pth_params;

void *tcpLoop(void *pthread_args) {
  pth_params *args = (pth_params *)pthread_args;

  int serverAddrLen = sizeof(args->serverAddress);

  while (1) {
    int clientFd;

    printf("waiting for tcp client connection\n");
    if ((clientFd =
             accept(args->serverFd, (struct sockaddr *)&args->serverAddress,
                    (socklen_t *)&serverAddrLen)) < 0) {
      perror("client tcp connection accept");
      exit(EXIT_FAILURE);
    }

    send(clientFd, AGENT_TYPE, strlen(AGENT_TYPE), 0);
    close(clientFd);
  }
}

void *httpLoop(void *pthread_args) {
  pth_params *args = (pth_params *)pthread_args;

  int serverAddrLen = sizeof(args->serverAddress);

  while (1) {
    int clientFd;
    printf("waiting for http client connection\n");
    if ((clientFd =
             accept(args->serverFd, (struct sockaddr *)&args->serverAddress,
                    (socklen_t *)&serverAddrLen)) < 0) {
      perror("client http connection accept");
      exit(EXIT_FAILURE);
    }

    char *content;
    concat(&content, 3, "# Container configuration\n\
hardware_id=[",
           HARDWARE_ID, "]\n");

    char *resp;
    httpResp(&resp, content);
    send(clientFd, resp, strlen(resp), 0);

    free(content);
    free(resp);
    close(clientFd);
  }
}

int main(int argc, char const *argv[]) {
  signal(SIGPIPE, SIG_IGN);

  printf("starting linux-agent %s\n", VERSION);

  int i = 0;
  int n;

  struct sockaddr_in serverAddress;
  int serverFd;
  startTCPSrv(&serverAddress, &serverFd, TCP_SERVER_PORT);

  pthread_t tid;

  pth_params params;
  params.serverFd = serverFd;
  params.serverAddress = serverAddress;
  pthread_create(&tid, NULL, tcpLoop, (void *)&params);

  struct sockaddr_in httpServerAddress;
  int httpServerFd;
  startTCPSrv(&httpServerAddress, &httpServerFd, HTTP_SERVER_PORT);

  pth_params httpParams;
  httpParams.serverFd = httpServerFd;
  httpParams.serverAddress = httpServerAddress;
  pthread_create(&tid, NULL, httpLoop, (void *)&httpParams);

  pthread_exit(NULL);

  return 0;
}
