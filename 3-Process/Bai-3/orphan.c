#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t orphan;
    orphan = fork();
    int status;
    if(orphan == 0)
    {
        for(int i = 0; i < 10; i++)
        {
            printf("PPID cua tien trinh con la: %d\n", getppid());
            sleep(1);
        }
    }
    else
    {
        
    }
}