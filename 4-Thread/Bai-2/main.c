#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long long counter = 0;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* incrementValue1(void* arg)
{
    pthread_mutex_lock(&mtx);
    for(int i = 0; i < 300000; i++)
    {
        counter++;
    }
    pthread_mutex_unlock(&mtx);
}

void* incrementValue2(void* arg)
{
    pthread_mutex_lock(&mtx);
    for(int i = 0; i < 300000; i++)
    {
        counter++;
    }
    pthread_mutex_unlock(&mtx);
}

void* incrementValue3(void* arg)
{
    pthread_mutex_lock(&mtx);
    for(int i = 0; i < 400000; i++)
    {
        counter++;
    }
    pthread_mutex_unlock(&mtx);
}

int main(int argc, char *argv[]) 
{
    pthread_t thread1, thread2, thread3;
    pthread_create(&thread1, NULL, incrementValue1, NULL);
    pthread_create(&thread2, NULL, incrementValue2, NULL);
    pthread_create(&thread3, NULL, incrementValue3, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    printf("Giá trị của couter là: %lld.\n", counter);
}