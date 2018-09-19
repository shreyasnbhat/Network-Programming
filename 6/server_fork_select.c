#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int MAX_QUEUE = 20;
int MAX_CLIENTS = 30;

int active_processes = 0;
int generated_processes = 0;

pthread_mutex_t lock;

void on_completion(int sig) {

        pid_t p;
        int status;
        wait(&status);

        pthread_mutex_lock(&lock);
        active_processes--;
        pthread_mutex_unlock(&lock);

        printf("Active: %d  Generated: %d\n",active_processes,generated_processes);
}

void error(char* msg) {
        perror(msg);
        exit(0);
}

void print_client_ip_port(void *socket_fd) {
        int sockfd = *(int *)socket_fd;
        char ip[16];
        unsigned int port;
        struct sockaddr_in my_addr;
        bzero(&my_addr, sizeof(my_addr));
        int len = sizeof(my_addr);
        getpeername(sockfd, (struct sockaddr *) &my_addr, &len);
        inet_ntop(AF_INET, &my_addr.sin_addr, ip, sizeof(ip));
        port = ntohs(my_addr.sin_port);

        printf("%s:%u connected\n", ip,port);
}


void *handler(void *socket_descriptor) {
        int sockfd = *(int *)socket_descriptor;
        int n;
        char buffer[256];
        bzero(buffer,256);

        print_client_ip_port((void *)&sockfd);

        while((n = read(sockfd,buffer,255)) > 0) {

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

                n = write(sockfd,message,strlen(message));

                if(n < 0) error("Error writing to socket");
        }

        return NULL;
}

int main(int argc, char const *argv[]) {

        if(argc < 2) {
                printf("Error! No Port Provided\n");
                exit(1);
        } else {

                signal(SIGCHLD, on_completion);
                fd_set master;
                int max_descriptor;

                FD_ZERO(&master);

                int client_sockets[MAX_CLIENTS];

                // Client socket init
                for(int i = 0; i < MAX_CLIENTS; i++) {
                        client_sockets[i] = 0;
                }

                if (pthread_mutex_init(&lock, NULL) != 0)
                {
                        printf("\n mutex init has failed\n");
                        return 1;
                }

                char buffer[256];
                struct sockaddr_in server_addr,client_addr;

                int socketfd =  socket(AF_INET,SOCK_STREAM,0);
                if(socketfd < 0) {
                        error("Error opening socket!\n");
                }

                bzero((char*) &server_addr,sizeof(server_addr));

                int port = atoi(argv[1]);
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(port);
                server_addr.sin_addr.s_addr = INADDR_ANY;

                if(bind(socketfd, (struct sockaddr*)&server_addr,sizeof(server_addr)) < 0) {
                        error("Error on binding");
                }

                listen(socketfd,MAX_QUEUE);

                int client_length = sizeof(client_addr);
                int acceptfd;

                while(1) {

                        FD_SET(socketfd,&master);
                        max_descriptor = socketfd;
                        
                        // Add client sockets to set
                        for(int i = 0; i < MAX_CLIENTS; i++) {
                                if(client_sockets[i] > 0)
                                        FD_SET(client_sockets[i],&master);

                                if(client_sockets[i] > max_descriptor)
                                        max_descriptor = client_sockets[i];
                        }

                        int activity = select(max_descriptor + 1, &master, NULL, NULL, NULL);

                        if(activity < 0) {
                                error("Error in select");
                        }

                        // Some incoming connection
                        if(FD_ISSET(socketfd,&master)) {
                                acceptfd = accept(socketfd,(struct sockaddr*)&client_addr,&client_length);

                                active_processes++;
                                generated_processes++;
                                pid_t pid = fork();

                                if(pid == -1) {
                                        error("Error on creating new process");
                                } else if(pid == 0) {
                                        // Child Process Work
                                        handler((void*)&acceptfd);
                                        exit(0);
                                } else {
                                        for (int i = 0; i < MAX_CLIENTS; i++)
                                        {
                                                if( client_sockets[i] == 0 )
                                                {
                                                        client_sockets[i] = acceptfd;
                                                        break;
                                                }
                                        }
                                        printf("Active: %d  Generated: %d\n",active_processes,generated_processes);
                                }

                        }
                }
        }
        return 0;
}
