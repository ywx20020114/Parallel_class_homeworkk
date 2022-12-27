#include<pmmintrin.h>
#include<time.h>
#include<xmmintrin.h>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
int N=128;


long long head, tail, freq;

using namespace std;



float** normal_gaosi(float **matrix) //û��SSE���еĸ�˹��ȥ��

{

    for (int k = 0; k < N; k++)

    {

        float tmp =matrix[k][k];

        for (int j = k; j < N; j++)

        {

            matrix[k][j] = matrix[k][j] / tmp;

        }

        for (int i = k + 1; i < N; i++)

        {

            float tmp2 = matrix[i][k];

            for (int j = k + 1; j < N; j++)

            {

                matrix[i][j] = matrix[i][j] - tmp2 * matrix[k][j];

            }

            matrix[i][k] = 0;

        }

    }

    return matrix;

}



void SSE_gaosi(float **matrix) //����SSE���еĸ�˹��ȥ��

{

    __m128 t1, t2, t3, t4;

    for (int k = 0; k < N; k++)

    {

        float tmp[4] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };

        t1 = _mm_loadu_ps(tmp);

        for (int j = N - 4; j >=k; j -= 4) //�Ӻ���ǰÿ��ȡ�ĸ�

        {

            t2 = _mm_loadu_ps(matrix[k] + j);

            t3 = _mm_div_ps(t2, t1);//����

            _mm_storeu_ps(matrix[k] + j, t3);

        }

        if (k % 4 != (N % 4)) //�����ܱ�4������Ԫ��

        {

            for (int j = k; j % 4 != ( N% 4); j++)

            {

                matrix[k][j] = matrix[k][j] / tmp[0];

            }

        }

        for (int j = (N % 4) - 1; j>= 0; j--)

        {

            matrix[k][j] = matrix[k][j] / tmp[0];

        }

        for (int i = k + 1; i < N; i++)

        {

            float tmp[4] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };

            t1 = _mm_loadu_ps(tmp);

            for (int j = N - 4; j >k;j -= 4)

            {

                t2 = _mm_loadu_ps(matrix[i] + j);

                t3 = _mm_loadu_ps(matrix[k] + j);

                t4 = _mm_sub_ps(t2,_mm_mul_ps(t1, t3)); //����

                _mm_storeu_ps(matrix[i] + j, t4);

            }

            for (int j = k + 1; j % 4 !=(N % 4); j++)

            {

                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];

            }

            matrix[i][k] = 0;

        }

    }

}


// �ش�����





//test

int main()

{
    //  test��Ԫ
    for(int i=0;i<10;i++){
        cout<<"���ݹ�ģΪ��"<<N<<endl;
        srand((unsigned)time(NULL));
        float **matrix = new float*[N];
        float **matrix2 = new float*[N];
        for (int i = 0; i<N; i++)
        {
            matrix[i] = new float[N];
            matrix2[i] = matrix[i];
        }
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                matrix[i][j] = rand() % 100;
            }
        }


        cout <<"��ʹ��SSE���еĸ�˹��ȥ��" << endl;
        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        float **B = normal_gaosi(matrix);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<  "�ܹ���ʱ��"<<(tail - head) * 1000.0 / freq << "ms" <<"    "<< endl;



        cout << "ʹ��SSE���еĸ�˹��ȥ��" << endl;
        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        SSE_gaosi(matrix2);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout << "�ܹ���ʱ�� " <<(tail - head) * 1000.0 / freq << "ms" <<"    "<< endl;
        N+=128;
    }




    return 0;

}
