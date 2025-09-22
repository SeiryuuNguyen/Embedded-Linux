#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;

int data, data_ready = 0;

void* producer(void* arg)
{
    
    for(int i = 0; i < 10; i++)
    {
        sleep(1);
        pthread_mutex_lock(&mtx);
        data = rand() % 100;
        printf("Producer tạo dữ liệu: %d\n", data);
        data_ready = 1;
        pthread_cond_signal(&cnd);
        pthread_mutex_unlock(&mtx);
    }
   
}

void* consumer(void* arg)
{
    
    for(int i = 0; i < 10; i++)
    {
        pthread_mutex_lock(&mtx);
        while(data_ready == 0)
        {
            pthread_cond_wait(&cnd, &mtx);
        }
        printf("Consumer nhận dữ liệu: %d\n", data);
        data_ready = 0;
        pthread_mutex_unlock(&mtx);
    }
    
}

int main()
{
    pthread_t Consumer, Producer;

    pthread_create(&Consumer, NULL, consumer, NULL);
    pthread_create(&Producer, NULL, producer, NULL);

    pthread_join(Consumer, NULL);
    pthread_join(Producer, NULL);
}
