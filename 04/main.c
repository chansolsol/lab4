#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_CLIENTS 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t message_ready = PTHREAD_COND_INITIALIZER;

char* messages[MAX_CLIENTS];
int num_clients = 0;

//클라이언트 구현
void* client_thread(void* arg) {
    int client_id = *((int*)arg);
    
    while (1) {
        pthread_mutex_lock(&mutex);
        messages[client_id] = "Hello from client!";
        printf("Client %d sent a message.\n", client_id);
        num_clients++;
        pthread_cond_signal(&message_ready); 
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}

//서버 구현
void* server_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (num_clients < MAX_CLIENTS) {
            pthread_cond_wait(&message_ready, &mutex); 
        }
        
        printf("Server broadcasting message to all clients:\n");
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            printf("Message to client %d: %s\n", i, messages[i]);
        }

        num_clients = 0; 
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t clients[MAX_CLIENTS], server;
    int client_ids[MAX_CLIENTS];

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_ids[i] = i;
        pthread_create(&clients[i], NULL, client_thread, &client_ids[i]);
    }

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 쓰레드 종료 대기
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(clients[i], NULL);
    }
    pthread_join(server, NULL);

    return 0;
}
