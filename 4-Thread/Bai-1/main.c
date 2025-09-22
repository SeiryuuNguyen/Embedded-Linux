#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* print_ID(void* arg)
{
    unsigned long ID;
    ID = pthread_self();
    printf("Thread với ID %lu đang chạy.\n", ID);
}

int main(int argc, char *argv[]) 
{
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, print_ID, NULL);
    pthread_create(&thread2, NULL, print_ID, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
}