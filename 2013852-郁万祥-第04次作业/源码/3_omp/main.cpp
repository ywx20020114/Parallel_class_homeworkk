#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <ctime>
#include <sys/time.h>
#include <algorithm>
#include <pmmintrin.h>


const int n = 2048;//固定矩阵规模，控制变量
const int maxN = n + 1; // 矩阵的最大值
float a[maxN][maxN];
float temp[maxN][maxN];//用于暂时存储a数组中的变量，控制变量唯一
const int THREAD_NUM = 4; //表示线程的个数
int seg = 40;//表示每次线程运行的行数，将来它会改变来探究最优任务分配方法
int next_task = 0;
int line = 0;//记录当前所依赖的行数
struct timeval startTime, stopTime;// timers


/**
 * 根据第i行的元素，消除j行的元素
 * @param i 根据的行数
 * @param j 要消元的行数
 */
void OMP_elimination(int i, int j) {
    float tep;
    __m128 div, t1, t2, sub;
    // 用temp暂存相差的倍数
    tep = a[j][i] / a[i][i];
    // div全部用于存储temp，方便后面计算
    div = _mm_set1_ps(tep);

    //每四个一组进行计算，思想和串行类似
    int k = n - 3;
    for (; k >= i + 1; k -= 4) {
        t1 = _mm_loadu_ps(a[i] + k);
        t2 = _mm_loadu_ps(a[j] + k);
        sub = _mm_sub_ps(t2, _mm_mul_ps(t1, div));
        _mm_store_ss(a[j] + k, sub);
    }
    //处理剩余部分
    for (k += 3; k >= i + 1; --k) {
        a[j][k] -= a[i][k] * tep;
    }
    a[j][i] = 0.00;
}

/**
 * 多线程消元函数,动态粗颗粒分配，每次分配seg个
 * @param parm
 */
void OMP_func() {
    int task = 0;
    while (true) {
#pragma omp critical //每次只能一个线程执行
        {
            task = next_task;
            next_task += seg;//每次分配seg个
        }
        if (task >= n) break;
        int min = task + seg < n ? task + seg : n;
        for (int i = task; i < min; ++i) {
            OMP_elimination(line, i);
        }
    }
}

//用于矩阵改变数值,为防止数据溢出,随机数的区间为100以内的浮点数
void change() {
    srand((unsigned) time(NULL));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            a[i][j] = (float) (rand() % 10000) / 100.00;
        }
    }
}


/**
 * 将a数组的数据存储到b数组中
 * @param a
 * @param b
 */
void store(float a[][maxN], float b[][maxN]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            b[i][j] = a[i][j];
        }
    }
}

int main(int arg, char *argv[]) {
    change();
    store(a, temp);
    // SSE算法消元设计
    for (; seg <= 400; seg += 40) {
        store(temp, a);//从temp中取数
        printf("seg: %d\n", seg);
        gettimeofday(&startTime, NULL);
        for (line = 0; line < n - 1; ++line) {
            next_task = line + 1;
# pragma omp parallel num_threads(THREAD_NUM)
            OMP_func();
        }
        gettimeofday(&stopTime, NULL);
        double trans_mul_time =
                (stopTime.tv_sec - startTime.tv_sec) * 1000 + (stopTime.tv_usec - startTime.tv_usec) * 0.001;
        printf("time: %lf ms\n", trans_mul_time);
    }
}
