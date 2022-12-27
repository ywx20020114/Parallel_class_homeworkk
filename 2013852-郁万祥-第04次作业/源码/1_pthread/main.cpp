#include <pthread.h>
#include <algorithm>
#include <stdio.h>
# include <stdlib.h>


double f(double x) {
    return x * x;
}

typedef struct {
    int threadId;
} threadParm_t;
const int THREAD_NUM = 5; //表示线程的个数
int next_task = 1;//从1开始
int seg = 50;//每次分50份
double totalSize = 0.000; //表示总面积
double h = 0.00;//表示间距
double a = 1.000, b = 8.000;
int n = 1000;
pthread_mutex_t barrier_mutex = PTHREAD_MUTEX_INITIALIZER;


void cal(int i) {
    //计算第i分
    double x = a + i * h;
    double myResult = f(x) * h;
    pthread_mutex_lock(&barrier_mutex);
    totalSize += myResult;
    pthread_mutex_unlock(&barrier_mutex);
}

void *SSE_pthread(void *parm) {
    int task = 0;
    while (true) {
        pthread_mutex_lock(&barrier_mutex);
        next_task += seg;
        task = next_task;
        pthread_mutex_unlock(&barrier_mutex);
        if (task - seg >= n) break;
        for (int i = task - seg; i < std::min(task, n); i++) {
            cal(i);
        }
    }
    pthread_exit(nullptr);
}

int main(int arg, char *argv[]) {
    pthread_t thread[THREAD_NUM];
    threadParm_t threadParm[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i++) {
        threadParm[i].threadId = i;
    }
    printf("Enter a, b, and n\n");
    scanf("%lf %lf %d", &a, &b, &n);
    //先计算两边的面积之和
    h = (b - a) / n;
    totalSize = h * (f(a) + f(b)) / 2;
    for (int i = 0; i < THREAD_NUM; i++) {
        pthread_create(&thread[i], nullptr, SSE_pthread, (void *) &threadParm[i]);
    }
    for (int k = 0; k < THREAD_NUM; k++) {
        pthread_join(thread[k], nullptr);
    }
    pthread_mutex_destroy(&barrier_mutex);
    printf("this is pthread\n");
    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.14e\n", a, b, totalSize);
    return 0;
}
