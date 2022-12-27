# include <mpi.h>
#include <stdio.h>
#include <math.h>

double totalSize = 0.00;//表示总面积
double gap = 0.00;
int total_num = 1000;
double begin_num = 1.000, end_num = 8.0000;

int min(int i, int num);

double f(double x) {
    return x * x;
}

double cal(int begin, int size) {
    double temp_size = 0.00;
    for (int i = begin; i < min(begin + size, total_num); ++i) {
        temp_size += (f(begin_num + gap * i) + f(begin_num + gap * (i + 1))) * gap / 2;
    }
    return temp_size;
}

int min(int i, int num) {
    if (i < num) {
        return i;
    }
    return num;
}


void accept_task(int thread_num) {
}

int main(int argc, char *argv[]) {
    double buf[10];
    gap = (end_num - begin_num) / total_num;
    int rank, thread_num;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &thread_num);
    //每个线程都分配这么(size*rand)多个任务,主线程不分配
    int each_thread_size = total_num / (thread_num - 1) + 1;
    if (rank != 0) {
        buf[rank] = cal((rank - 1) * each_thread_size, each_thread_size);
        MPI_Send(buf + rank, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    if (rank == 0) {
        for (int i = 1; i < thread_num; ++i) {
            double temp_size;
            MPI_Recv(&temp_size, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalSize += temp_size;
        }
        printf("total size: %lf\n", totalSize);
    }
    MPI_Finalize();
    return MPI_SUCCESS;
}
