#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main() {

    // FD's of the 2 pipes
    int fd1[2];  // Used to store two ends of first pipe
    int fd2[2];  // Used to store two ends of second pipe

    if (pipe(fd1)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }
    if (pipe(fd2)==-1)
    {
        fprintf(stderr, "Pipe Failed" );
        return 1;
    }


    if (fork()== 0) {
        char parent_str[100];
        read(fd1[0], parent_str, 100);
        printf("%d %s\n",getpid(),parent_str);

        close(fd1[0]);
        close(fd2[0]);

        int x;
        printf("Enter number to send from child to parent: ");
        scanf("%d",&x);

        char input_str[100];
        sprintf(input_str,"%d",x);

        write(fd2[1],input_str,strlen(input_str)+1);
        close(fd2[1]);

        exit(0);

     } else {

        close(fd1[0]);

        int x;
        printf("Enter number to send from parent to child: ");
        scanf("%d",&x);

        char input_str[100];
        sprintf(input_str,"%d",x);

        write(fd1[1],input_str,strlen(input_str)+1);
        close(fd1[1]);

        wait(NULL);

        close(fd2[1]);

        char output_str[100];
        read(fd2[0], output_str, 100);
        printf("%d %s\n",getpid(),output_str);
        close(fd2[0]);

    }

    return 0;
}
