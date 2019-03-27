#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "cJSON.h"

#define MAX_CONNECTIONS 10
#define MESSAGE_BUFFER 1024

typedef struct {
  int id;
  char username[25];
  int status;
  struct sockaddr_in addr;
} client;

client connections[MAX_CONNECTIONS];

int main(int argc, char**argv) {

}
