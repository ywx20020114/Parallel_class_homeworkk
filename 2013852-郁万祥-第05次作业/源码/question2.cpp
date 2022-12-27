#include<stdio.h>
#include<mpi.h>
#include <algorithm> 
#include <iostream> 
#include <vector>
#include <time.h>

using namespace std; 

const int N = 2048; 
vector<int> arr[N];


void init(void) { 
    srand(unsigned(time(nullptr)));  
    for (int i = 0; i < N; i++) 
    { 
        arr[i].resize(N);   
        for (int j = 0; j < N; j++)    
            arr[i][j] = rand(); 
    } 
}


void arr_sort(int myid, int numprocs) { 
    int r = myid, seg = N / numprocs; 
    for (int i = r * seg; i < (r + 1) * seg; i++)   
        sort(arr[i].begin(), arr[i].end());  
}


int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);
    int myid, numprocs;
    double start, end;

    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    start = MPI_Wtime();
    arr_sort(myid, numprocs);
    end = MPI_Wtime();

    if (myid == 0) {
        printf("ARR_NUM = %d \n", N);
        printf("MPI_arraysort time: %fs", end - start);
    }
    MPI_Finalize();
    return 0;
}
