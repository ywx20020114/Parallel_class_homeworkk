
/*
使用barrier接口

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 4
typedef struct
{
    int threadId;
} threadParm_t;
pthread_barrier_t barrier;
void *threadFunc(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
    pthread_barrier_wait(&barrier);
    fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    pthread_t thread[NUM_THREADS];
    threadParm_t threadParm[NUM_THREADS];
    int i;
    for (i=0; i<NUM_THREADS; i++)
    {
        threadParm[i].threadId = i;
        pthread_create(&thread[i], NULL, threadFunc, (void
                       *)&threadParm[i]);
    }
    for (i=0; i<NUM_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }
    pthread_barrier_destroy(&barrier);
    system("PAUSE");
    return 0;
}
*/

/*
使用忙等待和互斥量
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define NUM_THREADS 4

typedef struct {
    int threadId;
} threadParm_t;
int counter=0; //表示正在运行的线程数;

pthread_mutex_t barrier_mutex;

void *threadFunc(void *parm) {
    threadParm_t *p = (threadParm_t *) parm;
    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
    pthread_mutex_lock(&barrier_mutex);
    counter++;
    pthread_mutex_unlock(&barrier_mutex);
    while (counter < NUM_THREADS);
    fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_mutex_init(&barrier_mutex, NULL);
    pthread_t thread[NUM_THREADS];
    threadParm_t threadParm[NUM_THREADS];
    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        threadParm[i].threadId = i;
        pthread_create(&thread[i], NULL, threadFunc, (void
        *) &threadParm[i]);
    }
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }
    pthread_mutex_destroy(&barrier_mutex);
    return 0;
}
*/

// 使用信号量
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_THREADS 4
typedef struct
{
    int threadId;
} threadParm_t;
int counter = 0; //判断有多少线程抵达了路障
sem_t count_sem; //用于保护计数器
sem_t barrier_sem; //用于阻塞已经进入路障的线程。

void *threadFunc(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
    sem_wait(&count_sem);
    if (counter == NUM_THREADS - 1)
    {
        counter = 0;              // 计数器清零，以后可以重复使用
        sem_post(&count_sem);   // 计数器解锁， count_sem 值加 1
        for (int i = 0; i < NUM_THREADS - 1; sem_post(&barrier_sem), i++);// 解锁整个栅栏，
    }
    else
    {
        counter++;                // 计数器加一
        sem_post(&count_sem);   // 解锁计数器
        sem_wait(&barrier_sem); // 等待栅栏解锁
    }
    fprintf(stdout, "Thread %d has entered step 2.\n", p->threadId);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    sem_init(&count_sem, 0, 1);
    sem_init(&barrier_sem, 0, 0);
    pthread_t thread[NUM_THREADS];
    threadParm_t threadParm[NUM_THREADS];
    int i;
    for (i = 0; i < NUM_THREADS; i++)
    {
        threadParm[i].threadId = i;
        pthread_create(&thread[i], NULL, threadFunc, (void
                       *) &threadParm[i]);
    }
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(thread[i], NULL);
    }
    sem_destroy(&count_sem);
    sem_destroy(&barrier_sem);
    return 0;
}


