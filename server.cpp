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
#include "server.hpp"

using namespace std;
// for convenience
using json = nlohmann::json;

#define MAX_CONNECTIONS 10
#define MESSAGE_BUFFER 1024

client connections[MAX_CONNECTIONS];
int clientNum, id = 0;
vector<client *> clients;
static pthread_mutex_t list_lock;

int main(int argc, char**argv) {
  int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	pthread_t tid, pt;

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

  cout << "<[SERVER STARTED]>" << endl;

	if (pthread_mutex_init(&list_lock, NULL) != 0) { 
    handleError("Failed to initialize mutex\n"); 
	}

	/* Iniciar el servicio que nos dice que tanto lleva el user sin hacer nada */
	if (pthread_create(&pt, NULL, ping, NULL) != 0){
		cout << "No se pudo iniciar el servicio de ping" << endl;
		exit(1);
	}
	
  /* Empezamos a escuchar conexiones de clientes */
  while(1){
    socklen_t clilen = sizeof(cli_addr);
    connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);
    if (clientNum > MAX_CONNECTIONS) {
      cout << "Ya se llego al numero maximo de conexiones" << endl;
      close(connfd);
      continue;
    }

    printf("Incoming connection request:\n");
		printf("IP Adress: %d\n", cli_addr.sin_addr.s_addr);
		printf("Port: %d\n", cli_addr.sin_port);
    printf("ID: %d\n", connfd);
    client *cli = new client;
		cli->addr = cli_addr;
		cli->fd = connfd;
		cli->status = 0;
		cli->id = id++;
		cli->last_conection = 0;
		cli->username = to_string(cli->id);

		/* Fork thread */
		pthread_create(&tid, NULL, &handleSession, (void*)cli);
		pthread_detach(tid);

		/* Reduce CPU usage */
    sleep(1);
  }
	return 0;
}

void handleError(string ErrorMessage){
  perror(ErrorMessage.c_str());
  exit(EXIT_FAILURE);
}

void sendPrivateMessage(string message, string from, vector<int> to){
	json response;
	response["code"] = 201;
	response["data"]["from"] = from;
	response["data"]["message"] = message;
	string msg = response.dump();
	for (auto it = to.begin(); it != to.end(); it++){
		for(auto user: clients){
			if (user->id == *it) {
				write(user->fd, msg.c_str(), msg.length());
			}
		}
	}
}

void sendDiffusion(string message, string from) {
	json response;
	response["code"] = 201;
	response["data"]["from"] = from;
	response["data"]["message"] = message;
	string msg = response.dump();
	for(auto user: clients){
		write(user->fd, msg.c_str(), msg.length());
	}
}

void removeFromQueue(int id){
	pthread_mutex_lock(&list_lock);
	for(auto& pointer : clients) {
  	if (pointer->id == id) {
      delete pointer;
    	pointer = nullptr;
  	}
	}
	cout << "nada que ver aca" << endl;
	clients.erase(remove(clients.begin(), clients.end(), nullptr), clients.end());
	cout << "ni aca" << endl;
	pthread_mutex_unlock(&list_lock);
}

string getUsers(vector<int> userIds){
	json response;
	vector<json> users;
	response["code"] = 203;
	pthread_mutex_lock(&list_lock);
	for (auto it = userIds.begin(); it != userIds.end(); it++){
		for(auto user: clients){
			if (user->id == *it) {
				json us;
				us["id"] = user->id;
				us["username"] = user->username;
				us["status"] = user->status;
				users.push_back(us);
			}
		}
	}
	pthread_mutex_unlock(&list_lock);
	response["data"]["users"] = users;
	return response.dump();
}

string getAllUsers(){
	json response;
	vector<json> users;
	response["code"] = 203;
	pthread_mutex_lock(&list_lock);
	for(auto user: clients){
		json us;
		us["id"] = user->id;
		us["username"] = user->username;
		us["status"] = user->status;
		users.push_back(us);
	}
	pthread_mutex_unlock(&list_lock);
	response["data"]["users"] = users;
	return response.dump();
}

string getErrorResponse(string message){
	json response;
	response["code"] = 500;
	response["data"]["error_message"] = message;
	return response.dump();
}

string getBasicOk(string status){
	json response;
	response["code"] = 200;
	response["data"]["status"] = status;
	return response.dump();
}

/* Thread que itera los clientes y ve cuanto llevan desconectados */
void *ping(void *data){
	while(1){
		sleep(30);
		pthread_mutex_lock(&list_lock);
		for(auto user: clients){
			user->last_conection++;
			if (user->last_conection == 3) {
				cout << "User: " + user->username + " ha estado inactivo demasiado tiempo" << endl;
				user->status = 2;
			}
		}
		pthread_mutex_unlock(&list_lock);
	}
	return NULL;
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
	// Se lee lo primero que mando el cliente
  readBuff = read(cli->fd, buff, MESSAGE_BUFFER);
  printf("Valindado... %s\n", buff);
	auto j = json::parse(buff);
	cout << j.dump(4) << endl;
	json response, request;
	// Tiene la forma que queremos? Si si, responder el ok, si no responder con un error
	if (j.count("data") && j["code"] == 0) {
		printf("Estructura de peticion valida...\n");
		json payload = j["data"];
		if (payload.count("username") && payload["username"].is_string()) {
			username = payload["username"];
			printf("Bienvenido %s\n", username.c_str());
			cli->username = username;
		}
	} else {
		string msg = getErrorResponse("Invalid handshake");
		write(cli->fd, msg.c_str(), msg.length());
		close(cli->fd);
		return NULL;
	}
	response["code"] = 200;
	response["data"]["user"]["id"] = cli->id;
	response["data"]["user"]["username"] = cli->username;
	response["data"]["user"]["status"] = cli->status;
	write(cli->fd, response.dump().c_str(), response.dump().length());
	pthread_mutex_lock(&list_lock);
	clients.push_back(cli);
	pthread_mutex_unlock(&list_lock);
	memset(buff, 0, MESSAGE_BUFFER);
	while((readBuff = read(cli->fd, buff, sizeof(buff)-1)) > 0){
		request = json::parse(buff);
		cout << request.dump() << endl;
		if (!request.count("code") || !request.count("data")) {
			string errorResponse = getErrorResponse("Request mal estructurada");
			printf("Se ha recibido una request mal estructurada\n");
			write(cli->fd, errorResponse.c_str(), errorResponse.length());
			continue;
		}
		int code = request["code"];
		json data = request["data"];
		cli->last_conection = 0;
		switch (code) {
			// Va a enviar un mensaje, si viene vacio el arreglo se le envia a todos, de lo contrario solo a las personas en el arreglo
			case 1:{
				// Verificamos que en efecto la data tenga la forma deseada
				cout << data.dump()<< endl;
				if (!data.count("message") || !data.count("to")) {
					string errorResponse = getErrorResponse("Request de mensaje mal estructurada");
					write(cli->fd, errorResponse.c_str(), errorResponse.length());
					break;
				}
				string ok = getBasicOk("ok");
				vector<int> to = data["to"];
				string message = data["message"];
				if (to.empty()) {
					sendDiffusion(message, cli->username);
					write(cli->fd, ok.c_str(), ok.length());
					break;
				}
				write(cli->fd, ok.c_str(), ok.length());
				sendPrivateMessage(message, cli->username, to);
				break;
			}
			/* Get user */
			case 3:{
				// Verificamos que en efecto la data tenga la forma deseada
				if (!data.count("user")) {
					string errorResponse = getErrorResponse("Request de getUser mal estructurada");
					write(cli->fd, errorResponse.c_str(), errorResponse.length());
					break;
				}
				
				vector<int> users = data["user"];
				if (users.empty()) {
					string ok = getAllUsers();
					write(cli->fd, ok.c_str(), ok.length());
					break;
				}
				string ok = getUsers(users);
				write(cli->fd, ok.c_str(), ok.length());
				break;
			}
			case 4:{
				if (!data.count("new_status")) {
					string errorResponse = getErrorResponse("Request de getUser mal estructurada");
					write(cli->fd, errorResponse.c_str(), errorResponse.length());
					break;
				}
				int newStatus = data["new_status"];
				cli->status = newStatus;
				string ok = getBasicOk("ok");
				write(cli->fd, ok.c_str(), ok.length());
				break;
			}
			case 5:{
				string ok = getBasicOk("Oyasumi, client-kun");
				write(cli->fd, ok.c_str(), ok.length());
				cout << "Se ha desconectado el usuario: " + cli->username << endl;
				close(cli->fd);
				removeFromQueue(cli->id);
				return NULL;
				break;
			}
			default:
				/* No recognized code */
				printf("Error en una request realizada por el cliente %s\n", cli->username);
				string errorResponse = getErrorResponse("Codigo del request no reconocido");
				write(cli->fd, errorResponse.c_str(), errorResponse.length());
				break;
		}
		
		memset(buff, 0, MESSAGE_BUFFER);
	}
	cout << "Un usuario se ha desconectado de manera inesperada: " + cli->username << endl;
	close(cli->fd);
	removeFromQueue(cli->id);
	return NULL;
}

