#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>
#include <immintrin.h>
#include <windows.h>
#include <pthread.h>
using namespace std;
typedef struct
{
    int threadId;
} threadParm_t;
const int ARR_NUM = 10000;
const int ARR_LEN = 10000;
const int THREAD_NUM = 4                                 ;
const int seg = ARR_NUM / THREAD_NUM;
vector<int> arr[ARR_NUM];
pthread_mutex_t mutex;
long long head, freq; // timers
void init(void)
{
    int ratio;
    srand(unsigned(time(NULL)));
    for (int i = 0; i < ARR_NUM; i++)
    {
        arr[i].resize(ARR_LEN);
        if (i < seg) ratio = 0;
        else if (i < seg * 2) ratio = 32;
        else if (i < seg * 3) ratio = 64;
        else ratio = 128;
        if ((rand() & 127) < ratio)
            for (int j = 0; j < ARR_LEN; j++)
                arr[i][j] = ARR_LEN - j;
        else
            for (int j = 0; j < ARR_LEN; j++)
                arr[i][j] = j;
    }
}
void *arr_sort(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    int r = p->threadId;
    long long tail;
    for (int i = r * seg; i < (r + 1) * seg; i++)
        sort(arr[i].begin(), arr[i].end());
    pthread_mutex_lock(&mutex);
    QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    printf("Thread %d: %lfms.\n", r, (tail - head) * 1000.0 / freq);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}
    int next_arr = 0;
pthread_mutex_t mutex_task;
void *arr_sort_fine(void *parm)
{
    threadParm_t *p = (threadParm_t *) parm;
    int r = p->threadId;
    int task = 0;
    long long tail;
    while (1) {
    pthread_mutex_lock(&mutex_task);
    task = next_arr++;
    pthread_mutex_unlock(&mutex_task);
    if (task >= ARR_NUM) break;
    stable_sort(arr[task].begin(), arr[task].end());
    }
    pthread_mutex_lock(&mutex);
    QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    printf("Thread %d: %lfms.\n", r, (tail - head) * 1000.0 / freq);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}
int main(int argc, char *argv[])
{
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    init();
    mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_t thread[THREAD_NUM];
    threadParm_t threadParm[THREAD_NUM];
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    for (int i = 0; i < THREAD_NUM; i++)
    {
        threadParm[i].threadId = i;
        pthread_create(&thread[i], NULL, arr_sort_fine, (void *)&threadParm[i]);
    }
    for (int i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(thread[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
}
