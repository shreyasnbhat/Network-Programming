#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main() {
    int *integer = (int*) malloc(sizeof(int));
    printf("Enter the required number: ");
    scanf("%d",integer);

    printf("Process ID: %d\n",getpid());
    printf("Memory Address %p\n",integer);
    printf("Integer Value %d\n",*integer);

    if (fork()== 0) {
        printf("Child Process\n");
        *integer += 1;
        printf("%d %d\n",getpid(),*integer);

     } else {
        printf("Parent Process\n");
        *integer-=2;
        printf("%d %d\n",getpid(),*integer);

    }

    return 0;
}
