#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <ctime>
#include <sys/time.h>
#include <algorithm>
#include <pmmintrin.h>


const int n = 2048;//�̶������ģ�����Ʊ���
const int maxN = n + 1; // ��������ֵ
float a[maxN][maxN];
float temp[maxN][maxN];//������ʱ�洢a�����еı��������Ʊ���Ψһ
const int THREAD_NUM = 4; //��ʾ�̵߳ĸ���
int seg = 40;//��ʾÿ���߳����е���������������ı���̽������������䷽��
int next_task = 0;
int line = 0;//��¼��ǰ������������
struct timeval startTime, stopTime;// timers


/**
 * ���ݵ�i�е�Ԫ�أ�����j�е�Ԫ��
 * @param i ���ݵ�����
 * @param j Ҫ��Ԫ������
 */
void OMP_elimination(int i, int j) {
    float tep;
    __m128 div, t1, t2, sub;
    // ��temp�ݴ����ı���
    tep = a[j][i] / a[i][i];
    // divȫ�����ڴ洢temp������������
    div = _mm_set1_ps(tep);

    //ÿ�ĸ�һ����м��㣬˼��ʹ�������
    int k = n - 3;
    for (; k >= i + 1; k -= 4) {
        t1 = _mm_loadu_ps(a[i] + k);
        t2 = _mm_loadu_ps(a[j] + k);
        sub = _mm_sub_ps(t2, _mm_mul_ps(t1, div));
        _mm_store_ss(a[j] + k, sub);
    }
    //����ʣ�ಿ��
    for (k += 3; k >= i + 1; --k) {
        a[j][k] -= a[i][k] * tep;
    }
    a[j][i] = 0.00;
}

/**
 * ���߳���Ԫ����,��̬�ֿ������䣬ÿ�η���seg��
 * @param parm
 */
void OMP_func() {
    int task = 0;
    while (true) {
#pragma omp critical //ÿ��ֻ��һ���߳�ִ��
        {
            task = next_task;
            next_task += seg;//ÿ�η���seg��
        }
        if (task >= n) break;
        int min = task + seg < n ? task + seg : n;
        for (int i = task; i < min; ++i) {
            OMP_elimination(line, i);
        }
    }
}

//���ھ���ı���ֵ,Ϊ��ֹ�������,�����������Ϊ100���ڵĸ�����
void change() {
    srand((unsigned) time(NULL));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= n; j++) {
            a[i][j] = (float) (rand() % 10000) / 100.00;
        }
    }
}


/**
 * ��a��������ݴ洢��b������
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
    // SSE�㷨��Ԫ���
    for (; seg <= 400; seg += 40) {
        store(temp, a);//��temp��ȡ��
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
