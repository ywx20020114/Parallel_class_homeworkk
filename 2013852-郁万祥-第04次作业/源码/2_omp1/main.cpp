#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <ctime>
#include <sys/time.h>
#include <algorithm>


const int ARR_NUM = 1000; //固定数组大小
const int ARR_LEN = 4000; //固定数组长度
int THREAD_NUM = 4; //固定线程个数，改变粗颗粒分配大小
int seg = 10; //改变粗颗粒的分配大小
struct timeval startTime, stopTime;// timers
int arr[ARR_NUM][ARR_LEN];
int tempArr[ARR_NUM][ARR_LEN]; //用于暂时存储数组，控制变量唯一
int next_arr = 0;

void OMP_sort();

void init(int num) {
    srand(unsigned(time(nullptr)));
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < ARR_LEN; j++)
            arr[i][j] = rand();
    }
}

//使用冒泡排序，使变量唯一
void sort(int *a) {
    for (int i = 0; i < ARR_LEN - 1; ++i) {
        for (int j = i + 1; j < ARR_LEN; ++j) {
            if (a[j] < a[i]) {
                int temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }
}

void store(int a[][ARR_LEN], int b[][ARR_LEN], int num) {
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < ARR_LEN; j++)
            b[i][j] = a[i][j];
    }
}


int main(int argc, char *argv[]) {
    init(ARR_NUM);
    store(arr, tempArr, ARR_NUM);
    for (; seg <= 100; seg += 10) {
        printf("seg: %d\n",seg);
        store(tempArr, arr, ARR_NUM);
        next_arr = 0;
        gettimeofday(&startTime, NULL);
# pragma omp parallel num_threads(THREAD_NUM)
        OMP_sort();
        gettimeofday(&stopTime, NULL);
        double trans_mul_time =
                (stopTime.tv_sec - startTime.tv_sec) * 1000 + (stopTime.tv_usec - startTime.tv_usec) * 0.001;
        printf("time :%lf ms\n", trans_mul_time);

    }
    return 0;
} /* main*/

void OMP_sort() {
    int task = 0;
    while (true) {
#pragma omp critical //每次只能一个线程执行
        task = next_arr += seg;//每次动态分配seg个
        if (task - seg >= ARR_NUM) break;
        int min = task < ARR_NUM ? task : ARR_NUM;
        for (int i = task - seg; i < min; ++i) {
            sort(arr[i]);
        }
    }
} /* Trap*/
