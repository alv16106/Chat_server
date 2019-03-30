#include <stdio.h>
#include <sys/socket.h> //For Sockets
#include <stdlib.h>
#include <netinet/in.h> //For the AF_INET (Address Family)
#include <string.h>
#include <arpa/inet.h>

struct sockaddr_in serv; //This is our main socket variable.
int fd; //This is the socket file descriptor that will be used to identify the socket
int conn; //This is the connection file descriptor that will be used to distinguish client connections.
char message[100] = ""; //This array will store the messages that are sent by the server

int main(int argc, char const *argv[])
{
  fd = socket(AF_INET, SOCK_STREAM, 0);

  serv.sin_family = AF_INET;
  serv.sin_port = htons(5000);

  inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr); //This binds the client to localhost

  connect(fd, (struct sockaddr *)&serv, sizeof(serv)); //This connects the client to the server.

  while(1) {
    printf("Enter a message: ");
    fgets(message, 100, stdin);
    send(fd, message, strlen(message), 0);
    int socket_fd, response;
    char message[1024];
        // Print received message
    while(1) {
      response = recvfrom(fd, message, 1024, 0, NULL, NULL);
      if (response == -1) {
        printf("error");
        break;
      } else if (response == 0) {
        printf("\nPeer disconnected\n");
        break;
      } else {
        printf("\nServer> %s", message);
        fflush(stdout); // Make sure "User>" gets printed
      }
    }
    //An extra breaking condition can be added here (to terminate the while loop)
  }
  return 0;
}
