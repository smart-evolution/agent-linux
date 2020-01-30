#include "strings.h"
#include <stdlib.h>
#include <string.h>

void httpResp(char **resp, char *content) {
  char *contentLen = malloc(6);
  sprintf(contentLen, "%lu", strlen(content));

  concat(resp, 4,
         "HTTP/1.1 200 OK\n"
         "Content-Type: text/html\n"
         "Content-Length: ",
         contentLen, "\n\n", content);

  free(contentLen);
}