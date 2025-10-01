#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int seconds = 0;

void timer_handler(int sig)
{
    seconds++;
    printf("Timer %d seconds\n", seconds);
    if(seconds >= 10)
    {
        exit(0);
    }
    alarm(1);
}

int main()
{
    signal(SIGALRM, timer_handler);
    alarm(1);
    while(1)
    {
        pause();
    }
}