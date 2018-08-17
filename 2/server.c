#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>

void error(char* msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char const *argv[]) {

  if(argc < 2) {
    printf("Error! No Port Provided\n");
    exit(1);
  } else {
    char buffer[256];
    struct sockaddr_in server_addr,client_addr;

    // socketfd is Socket File Descriptor Entry
    int socketfd =  socket(AF_INET,SOCK_STREAM,0);
    if(socketfd < 0) {
      printf("Error opening socket!\n");
      exit(1);
    }

    // Sets server address buffer to all zeroes
    bzero((char*) &server_addr,sizeof(server_addr));

    int port = atoi(argv[1]);

    // Set server address
    server_addr.sin_family = AF_INET;

    // Done to convert port into the network byte format
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind to socket
    if(bind(socketfd, (struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
      error("Error on binding");
    }

    // Socket listens,5 is the size of the backlog queue
    listen(socketfd,5);

    // accept connections
    while(1) {
        int client_length = sizeof(client_addr);
        int acceptfd = accept(socketfd,(struct sockaddr*)&client_addr,&client_length);
        if(acceptfd < 0){
          error("Error on accept!");
        }

        bzero(buffer,256);
        int n = read(acceptfd,buffer,255);

        if(n < 0) error("Error reading from socket");

        // Message generation
        char* greeting = "Good Morning, ";
        char message[100];
        strcpy(message,greeting);
        strcat(message,buffer);
        message[strlen(buffer) + 13] = '!';
        message[strlen(buffer) + 14] = '\0';

        n = write(acceptfd,message,strlen(message));
        if(n < 0) error("Error writing to socket");
    }
  }

  return 0;
}