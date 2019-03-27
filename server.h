#include <netinet/in.h>


typedef struct {
  int id;
  char username[25];
  int status;
  int fd;
  struct sockaddr_in addr;
} client;

void handleError(char* message);
void *ping(void *data);
void *handleSession(void *data);
char *sendMessage(char *message, int from, int to);
char *sendDiffusion(char *message, int id);
char *okHandshake(struct sockaddr_in conn);
void handleError(char *ErrorMessage);
void addToQueue(client *cl);
void removeFromQueue(int id);
