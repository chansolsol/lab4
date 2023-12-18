#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3

void *thread_function(void *arg) {
    int thread_num = *((int *)arg);
    int i;

    printf("쓰레드 %d: 시작\n", thread_num);
    for (i = 0; i < 5; ++i) {
        printf("쓰레드 %d: 동작 %d\n", thread_num, i);
        sleep(1);
    }

    printf("쓰레드 %d: 종료\n", thread_num);
    pthread_exit(arg);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code, i;

    // 쓰레드 생성
    for (i = 0; i < NUM_THREADS; ++i) {
        thread_args[i] = i;
        result_code = pthread_create(&threads[i], NULL, thread_function, (void *)&thread_args[i]);
    }

    // 일정 시간 대기 후 쓰레드 취소
    sleep(3);
    for (i = 0; i < NUM_THREADS; ++i) {
        result_code = pthread_cancel(threads[i]);
    }

    printf("모든 쓰레드 종료\n");
    return EXIT_SUCCESS;
}

