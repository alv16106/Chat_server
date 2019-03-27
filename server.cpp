#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <stdlib.h>
#include<iostream> 
#include<string> // for string class 
#include "server.h"

using namespace std;
// for convenience
using json = nlohmann::json;

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
	return 0;
}

void handleError(char *ErrorMessage){
  perror(ErrorMessage);
  exit(EXIT_FAILURE);
}

string getErrorResponse(string message){
	json response;
	response["code"] = 500;
	response["data"]["error_message"] = message;
	return response.dump();
}

// Thread que maneja las peticiones de UN usuario
void *handleSession(void *data){
	// De que tamaño puede mandar las cosas ese usuario
  char buff[MESSAGE_BUFFER];
	int readBuff;
	string username;
	// Ya que entro se aumenta en contador de uids
	id++;
	client *cli = (client *)data;
	// Vemos si lo primero que hace un cliente es un handshake
  printf("Esperando Handshake...\n");
  readBuff = read(cli->fd, buff, MESSAGE_BUFFER);
  printf("Valindado... %s\n", buff);
	auto j = json::parse(buff);
	cout << j.dump(4) << endl;
	json response;
	if (j.count("data") && j["code"] == 0) {
		printf("Estructura de peticion valida...\n");
		json payload = j["data"];
		if (payload.count("username") && payload["username"].is_string()) {
			username = payload["username"];
			printf("Bienvenido %s\n", username.c_str());
		}
	} else {
		string msg = getErrorResponse("Invalid handshake");
		write(cli->fd, msg.c_str(), msg.length());
		close(cli->fd);
		return NULL;
	}
	
	
  strcpy(cli->username, username.c_str());
  printf("El username ahora es: %s", cli->username);
}
