#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid, wpid;
    pid = fork();
    int status;
    if(pid == 0)
    {
        printf("PID cua tien trinh con la: %d\n", getpid());
        exit(10);
    }
    else
    {
        wpid = wait(&status);
        printf("PID cua tien trinh cha la: %d\n", getpid());
        if(WIFEXITED(status))
        {
            printf("Ma ket thuc cua tien trinh con la: ̀%d\n", WEXITSTATUS(status));
            printf("wpid la: %d", wpid);
        }
    }
    return 0;
}