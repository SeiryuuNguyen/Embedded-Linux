#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

int data = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* reader(void* arg)
{
    int id = *((int*)arg);
    for(int i = 0; i < 10; i++)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("Reader %d doc du lieu: %d\n", id, data);
        pthread_rwlock_unlock(&rwlock);
        usleep(250000);
    }
}

void* writer(void* arg)
{
    int id = *((int*)arg);
    for(int i = 0; i < 10; i++)
    {
        pthread_rwlock_wrlock(&rwlock);
        data++;
        printf("Writer %d ghi du lieu: %d\n", id, data);
        pthread_rwlock_unlock(&rwlock);
        usleep(250000);
    }
}

void main()
{
    pthread_t readers[5], writers[2];
    int r_idx[5], w_idx[2];

    for(int i = 0; i < 2; i++)
    {
        w_idx[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, &w_idx[i]);
    }

    for(int i = 0; i < 5; i++)
    {
        r_idx[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &r_idx[i]);
    }

    // for(int i = 0; i < 2; i++)
    // {
    //     w_idx[i] = i + 1;
    //     pthread_create(&writers[i], NULL, writer, &w_idx[i]);
    // }
    
    for(int i = 0; i < 5; i++)
    {
        pthread_join(readers[i], NULL);
    }
    for(int i = 0; i < 2; i++)
    {
        pthread_join(writers[i], NULL);
    }
}