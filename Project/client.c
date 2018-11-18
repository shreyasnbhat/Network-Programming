#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

void error(const char *msg) { perror(msg); exit(0); }

int main(int argc,char *argv[])
{
    /* first what are we going to send and where are we going to send it? */
    int portno = 80;
    char host[100];
    char path[1000];
    char *message_fmt = "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n";

    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd, bytes, sent, received, total;
    char message[1024],response[4096];

    if (argc < 2) { puts("Parameters: -u URL"); exit(0); }
    else {
      if(argc == 2 && strcmp(argv[1],"-u") == 0) {
        printf("No URL provided");
        exit(0);
      } else if(argc == 2 && strcmp(argv[1],"-u") != 0) {
        printf("Only -u arguement supported. Usage ./test -u url\n");
        exit(0);
      }
    }

    // Path Parsing
    int counter = 0;
    int pathctr = 0;
    int i;
    int flag = 0;
    for(i = 7; i < strlen(argv[2]); i++) {
        if(flag == 0 && argv[2][i] == '/') {
          flag = 1;
        } else if(flag == 1) {
          path[pathctr++] = argv[2][i];
        } else {
        host[counter++] = argv[2][i];
      }
    }

    sprintf(message,message_fmt,path,host);
    printf("Request:\n%s\n------------------------\n",message);

    /* create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Error opening socket");

    /* lookup the ip address */
    server = gethostbyname(host);
    if (server == NULL) error("Error no such host");

    /* fill in the structure */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

    /* connect the socket */
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("Error connecting");

    /* send the request */
    total = strlen(message);
    bytes = write(sockfd,message+sent,total-sent);
    if (bytes < 0)
          error("Error writing message");

    /* receive the response */
    memset(response,0,sizeof(response));
    total = sizeof(response)-1;
    received = 0;
    do {
        bytes = read(sockfd,response+received,total-received);
        if (bytes < 0)
            error("Error reading response");
        if (bytes == 0)
            break;
        received+=bytes;
    } while (received < total);

    if (received == total)
        error("Unable to store complete response");

    /* close the socket */
    close(sockfd);

    char errorCode[3];
    errorCode[0] = response[9];
    errorCode[1] = response[10];
    errorCode[2] = response[11];
    int code = atoi(errorCode);

    if(code != 200) {
      printf("HTTP Status Code: %d\nFile could not be retreived",code);
    } else {
      FILE *fptr;
      fptr = fopen("response.txt","w");\
      fprintf(fptr,"%s",response);
      fclose(fptr);
      printf("Response file created: response.txt");
    }
    return 0;
}
