#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>

void error(char* msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char const *argv[]) {

  int socketfd, port, n;
  struct sockaddr_in server_addr;

  // Defines a host computer on the internet
  struct hostent *server;

  char buffer[256];
  if(argc < 2) {
    printf("Usage port\n");
    exit(0);
  }

  port = atoi(argv[1]);
  socketfd = socket(AF_INET,SOCK_STREAM,0);
  if(socketfd < 0) {
    error("Error opening socket!");
  }

  server = gethostbyname("localhost");
  if(server == NULL) {
    printf("Error no such host\n");
    exit(0);
  }

  bzero((char *) &server_addr,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,(char*)&server_addr.sin_addr.s_addr,server->h_length);
  server_addr.sin_port = htons(port);

  if(connect(socketfd,&server_addr,sizeof(server_addr)) < 0)
    error("Error connecting to host!");

  printf("What is your name? ");
  bzero(buffer,256);
  fgets(buffer,255,stdin);
  n = write(socketfd,buffer,strlen(buffer));
  if(n < 0) error("Error writing to socket!");
  bzero(buffer,256);

  n = read(socketfd,buffer,255);
  if(n < 0) error("Error reading from socket!");
  printf("%s\n",buffer);
  return 0;


}
