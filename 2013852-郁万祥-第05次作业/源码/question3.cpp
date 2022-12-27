#include <stdio.h>
#include <mpi.h>
#include <ctime>
#include <sys/time.h>
#include <algorithm>


const int n = 1024*4;//固定矩阵规模，控制变量
const int maxN = n + 1; // 矩阵的最大值
float a[maxN][maxN];
float temp[maxN][maxN];//用于暂时存储a数组中的变量，控制变量唯一
int next_task = 0;
int seg;
int line = 0;//记录当前所依赖的行数
struct timeval startTime, stopTime;// timers


/**
 * 根据第i行的元素，消除j行的元素
 * @param i 根据的行数
 * @param j 要消元的行数
 */
void OMP_elimination(int i, int j) {
    //求出相差倍数
    float temp = a[j][i] / a[i][i];
    //遍历这一行的所有值，将i后面的数值依次减去相对应的值乘以倍数
    for (int k = i + 1; k <= n; ++k) {
        a[j][k] -= a[i][k] * temp;
    }
    //第i个为0
    a[j][i] = 0.00;
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


int main(int argc, char *argv[]) {
    change();
    int rank, thread_num;
    gettimeofday(&startTime, NULL);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &thread_num);
    MPI_Status status;
    int ready;
    bool done = false;
    if (rank == 0) {
        printf("size : %d\n", n);
        for (line = 0; line < n - 1; ++line) {
            next_task = line + 1;
            seg = (n - next_task) / (thread_num - 1) + 1;
            for (int i = 1; i < thread_num; i++) {
                int task = (i - 1) * seg + next_task;
                MPI_Send(&task, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            //等待所有线程
            for (int j = 1; j < thread_num; ++j) {
                MPI_Recv(&ready, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            }
        }
        printf("all work done!!!!!\n");
        done = true;
        gettimeofday(&stopTime, NULL);
        double trans_mul_time =
                (stopTime.tv_sec - startTime.tv_sec) * 1000 + (stopTime.tv_usec - startTime.tv_usec) * 0.001;
        printf("time: %lf ms\n", trans_mul_time);
    } else {
        while (!done) {
            int task;
            MPI_Recv(&task, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            int min = task + seg < n ? task + seg : n;
            for (int i = task; i < min; ++i) {
                OMP_elimination(line, i);
            }
            MPI_Send(&ready, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    return 0;
}
