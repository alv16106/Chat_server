#include <netinet/in.h>
#include<string>


typedef struct {
  int id;
  std::string username;
  int status;
  int fd;
  int last_conection;
  struct sockaddr_in addr;
} client;

void handleError(std::string message);
void *ping(void *data);
void *handleSession(void *data);
void sendPrivateMessage(std::string message, std::string from, int to);
void sendDiffusion(std::string message, std::string from);
char *okHandshake(struct sockaddr_in conn);
void addToQueue(client *cl);
void removeFromQueue(int id);
