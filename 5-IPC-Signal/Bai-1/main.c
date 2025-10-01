#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int count = 0;

void handler(int sig)
{
    count++;
    printf("SIGINT received\n");
}

int main()
{
    signal(SIGINT, handler);
    while(1)
    {
        if(count >= 3)
        {
            exit(0);
        }
        printf("Embedded Linux\n");
        sleep(1);
    }
}