#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int signal_count = 0;

void handle_sigusr1(int sig) 
{
    printf("Received signal from parent\n");
}

int main() {
    pid_t pid = fork();

    if (pid == 0) 
    {
        signal(SIGUSR1, handle_sigusr1); 

        while (1) 
        {
            pause(); 
        }
    } 
    else 
    {
        for (int i = 0; i < 6; i++) 
        {
            sleep(2); 
            kill(pid, SIGUSR1);
        }

        kill(pid, SIGTERM); 
    }

    return 0;
}
