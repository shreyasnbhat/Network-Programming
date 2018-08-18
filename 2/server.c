#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

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

    /* socketfd is Socket File Descriptor Entry
    * AF_INET is used to specify the address family which is IPv4
    * SOCK_STREAM is used to specify a connection based protocol i.e TCP
    * 0 specifies the protocol value for IP, which is visible in the IP Header in the protocol field
    */
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

    /* Bind to socket
    * socketfd is the socket descriptor for the created socket
    * server_addr is the pointer to a sockaddr structure containing the name that is to be bound to socket
    * The third arguement is the length of this address
    */
    if(bind(socketfd, (struct sockaddr*)&server_addr,sizeof(server_addr)) < 0){
      error("Error on binding");
    }

    /* Socket listens to incoming connections
    * socketfd is the socket descriptor for the created socket
    * 5 is the maximum length that the queue of pending connections may grow to. Called as a backlog queue.
    */
    listen(socketfd,5);

    // accept connections
    while(1) {
        int client_length = sizeof(client_addr);


        /* Accept connection
        * socketfd is the socket descriptor for the created socket
        * client_addr is the pointer to a sockaddr structure containing the name of the connecting entity
        * client_length is the length of client_addr
        * acceptfd is the new socket descriptor created for the first connection in the queue
        */
        int acceptfd = accept(socketfd,(struct sockaddr*)&client_addr,&client_length);
        if(acceptfd < 0){
          error("Error on accept!");
        }

        bzero(buffer,256);

        /* Read from socket buffer
        * acceptd is the socket descriptor for the current connection
        * buffer is the pointer to the socket buffer where all socket data is present
        * 255 is the socket buffer size
        */
        int n = read(acceptfd,buffer,255);

        if(n < 0) error("Error reading from socket");

        // Message generation
        time_t curtime;
        struct tm *loc_time;
        curtime = time (NULL);
        loc_time = localtime (&curtime);
        int hour = loc_time->tm_hour;

        char* greeting;

        if( hour >= 4 && hour < 12) {
            greeting = "Good Morning, ";
        } else if( hour >= 12 && hour < 17) {
            greeting = "Good Afternoon, ";
        } else if( hour >= 17 && hour < 20) {
            greeting = "Good Evening, ";
        } else if( hour >= 20 || hour < 4) {
            greeting = "Good Night, ";
        }

        char message[100];
        buffer[strlen(buffer)- 1] = '!';
        int k=sprintf (message, "%s%s\0", greeting,buffer);

        /* Write to socket
        * acceptfd is the socket descriptor for the current connection
        * message is the pointer to the data to be written
        * strlen(message) is the length of the data to be written
        */
        n = write(acceptfd,message,strlen(message));
        if(n < 0) error("Error writing to socket");
    }
  }

  return 0;
}
