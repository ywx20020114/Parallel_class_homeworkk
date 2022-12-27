
/*
ʹ��barrier�ӿ�

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
ʹ��æ�ȴ��ͻ�����
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define NUM_THREADS 4

typedef struct {
    int threadId;
} threadParm_t;
int counter=0; //��ʾ�������е��߳���;

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

// ʹ���ź���
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_THREADS 4
typedef struct
{
    int threadId;
} threadParm_t;
int counter = 0; //�ж��ж����̵ִ߳���·��
sem_t count_sem; //���ڱ���������
sem_t barrier_sem; //���������Ѿ�����·�ϵ��̡߳�

void *threadFunc(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    fprintf(stdout, "Thread %d has entered step 1.\n", p->threadId);
    sem_wait(&count_sem);
    if (counter == NUM_THREADS - 1)
    {
        counter = 0;              // ���������㣬�Ժ�����ظ�ʹ��
        sem_post(&count_sem);   // ������������ count_sem ֵ�� 1
        for (int i = 0; i < NUM_THREADS - 1; sem_post(&barrier_sem), i++);// ��������դ����
    }
    else
    {
        counter++;                // ��������һ
        sem_post(&count_sem);   // ����������
        sem_wait(&barrier_sem); // �ȴ�դ������
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


