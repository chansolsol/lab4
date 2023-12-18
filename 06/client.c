#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#define BUF_SIZE 100
#define MYPORT 8080

void *send_msg(void *arg);
void *receive_msg(void *arg);
void error_handling(char *msg);

struct ThreadArgs {
    int sock;
    char *name;
};

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;

    if (argc != 3) {
        printf("Usage: %s <ip> <name>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(MYPORT);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    printf("Chat started.\n");

    struct ThreadArgs args;
    args.sock = sock;
    args.name = argv[2];

    pthread_create(&snd_thread, NULL, send_msg, (void *)&args);
    pthread_create(&rcv_thread, NULL, receive_msg, (void *)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

void *send_msg(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int sock = args->sock;
    char *name = args->name;
    char msg[BUF_SIZE];
    char msg_form[BUF_SIZE];

    while (1) {
        fgets(msg, BUF_SIZE, stdin);
        sprintf(msg_form, "[%s/%s]", name, msg);
        write(sock, msg_form, strlen(msg_form));
    }
    return NULL;
}

void *receive_msg(void *arg) {
    int sock = *((int *)arg);
    char msg[BUF_SIZE];

    while (1) {
        int str_len = read(sock, msg, BUF_SIZE - 1);
        if (str_len == -1)
            return (void *)-1;
        msg[str_len] = '\0';
        fputs(msg, stdout);
    }
    return NULL;
}

void error_handling(char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}