#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include "cJSON.h"
#include "server.h"

#define MAX_CONNECTIONS 10
#define MESSAGE_BUFFER 1024

client connections[MAX_CONNECTIONS];
int clientNum, id = 0;

int main(int argc, char**argv) {
  int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	pthread_t tid;

	/* Socket settings */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(5000);

	/* Bind */
	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		handleError("Socket binding failed");
	}

	/* Listen */
	if(listen(listenfd, MAX_CONNECTIONS) < 0){
		handleError("Socket listening failed");
	}

  printf("<[SERVER STARTED]>\n");

  /* Empezamos a escuchar conexiones de clientes */
  while(1){
    socklen_t clilen = sizeof(cli_addr);
    connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
    if (clientNum > MAX_CONNECTIONS) {
      printf("Ya se llego al numero maximo de conexiones\n");
      close(connfd);
      continue;
    }

    printf("Incoming connection request:\n");
		printf("IP Adress: %d\n", cli_addr.sin_addr.s_addr);
		printf("Port: %d\n", cli_addr.sin_port);
    printf("ID: %d\n", connfd);
    client *cli = (client *)malloc(sizeof(client));
		cli->addr = cli_addr;
		cli->fd = connfd;
		cli->id = id++;
		sprintf(cli->username, "%d", cli->id);

		/* Fork thread */
		pthread_create(&tid, NULL, &handleSession, (void*)cli);

		/* Reduce CPU usage */
    sleep(1);
  }
}

void handleError(char *ErrorMessage){
  perror(ErrorMessage);
  exit(EXIT_FAILURE);
}

void *handleSession(void *data){
  char buff[MESSAGE_BUFFER];
	int readBuff;

	id++;
	client *cli = (client *)data;
  printf("Esperando Handshake...\n");
  readBuff = read(cli->fd, buff, MESSAGE_BUFFER);
  printf("Valindado... %s\n", buff);
  strcpy(cli->username, buff);
  printf("El username ahora es: %s", cli->username);
}

