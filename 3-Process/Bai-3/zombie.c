#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t zombie;
    zombie = fork();
    int status;
    if(zombie == 0)
    {

    }
    else
    {
       sleep(15); 
    }
}