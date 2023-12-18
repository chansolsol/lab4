#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5 

int buffer[BUFFER_SIZE]; 
int in = 0; 
int out = 0; 
int itemCount = 0; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t can_produce = PTHREAD_COND_INITIALIZER; 
pthread_cond_t can_consume = PTHREAD_COND_INITIALIZER; 

// 생산자 쓰레드의 동작 구현
void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (itemCount == BUFFER_SIZE) {
            pthread_cond_wait(&can_produce, &mutex);
        }
        buffer[in] = rand() % 100; 
        printf("생산 %d\n", buffer[in]);
        in = (in + 1) % BUFFER_SIZE;
        itemCount++;

        pthread_cond_signal(&can_consume); 
        pthread_mutex_unlock(&mutex);

        // 생산자 대기
        sleep(rand() % 2);
    }
    return NULL;
}

// 소비자 쓰레드의 동작 구현
void *consumer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (itemCount == 0) {
            pthread_cond_wait(&can_consume, &mutex);
        }
        int item = buffer[out];
        printf("소비 %d\n", item);
        out = (out + 1) % BUFFER_SIZE;
        itemCount--;

        pthread_cond_signal(&can_produce); 
        pthread_mutex_unlock(&mutex);

        // 소비자 대기
        sleep(rand() % 2);
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;

    //생산자 소비자 쓰레드 생성
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    return 0;
}
