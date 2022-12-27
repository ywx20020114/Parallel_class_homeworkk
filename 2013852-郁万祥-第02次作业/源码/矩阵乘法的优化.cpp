#include <stdio.h>
#include <pmmintrin.h>
#include <stdlib.h>
#include <algorithm>
#include <windows.h>
#include<iostream>
using namespace std;
const int maxN =1024;
int T = 64;
int n;float a[maxN][maxN];
float b[maxN][maxN];
float c[maxN][maxN];
long long head, tail, freq;


//  ���о����time[0][n] = tail-head;��
void mul(int n, float a[][maxN], float b[][maxN], float c[][maxN])
{
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            c[i][j] = 0.0;
            for (int k = 0; k < n; ++k)
            {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

//cache�Ż�
void trans_mul(int n, float a[][maxN], float b[][maxN], float c[][maxN])
{
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            c[i][j] = 0.0;
            for (int k = 0; k < n; ++k)
            {
                c[i][j] += a[i][k] * b[j][k];
            }
        }
    }
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
}
// SSE�汾
void sse_mul(int n, float a[][maxN], float b[][maxN], float c[][maxN])
{
    __m128 t1, t2, sum;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            c[i][j] = 0.0;
            sum = _mm_setzero_ps();
            for (int k = n -4; k >= 0; k -= 4)
            {     // sum every 4 elements
                    t1 = _mm_loadu_ps(a[i] + k);
                    t2 = _mm_loadu_ps(b[j] + k);
                    t1 = _mm_mul_ps(t1, t2);
                    sum = _mm_add_ps(sum, t1);
            }
            sum = _mm_hadd_ps(sum, sum);
            sum = _mm_hadd_ps(sum, sum);
            _mm_store_ss(c[i] + j, sum);
            for (int k = (n % 4) -1; k >= 0; --k)
            {    // handle the last n%4 elements
                c[i][j] += a[i][k] * b[j][k];

            }
        }
    }
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
}
// ��Ƭ����
void sse_tile(int n, float a[][maxN], float b[][maxN], float c[][maxN])
{
    __m128 t1, t2, sum;float t;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < i; ++j)
            swap(b[i][j], b[j][i]);
    for (int r = 0; r < n / T; ++r)
        for (int q = 0; q < n / T; ++q)
        {
            for (int i = 0; i < T; ++i)
                for (int j = 0; j < T; ++j)
                    c[r * T + i][q * T + j] = 0.0;
            for (int p = 0; p < n / T; ++p)
            {
                for (int i = 0; i < T; ++i)
                    for (int j = 0; j < T; ++j)
                    {
                        sum = _mm_setzero_ps();
                        for (int k = 0; k < T; k += 4)
                        {     //sum every 4th elements
                            t1 = _mm_loadu_ps(a[r * T + i] + p * T + k);
                            t2 = _mm_loadu_ps(b[q * T + j] + p * T + k);
                            t1 = _mm_mul_ps(t1, t2);sum = _mm_add_ps(sum, t1);
                        }
                        sum = _mm_hadd_ps(sum, sum);
                        sum = _mm_hadd_ps(sum, sum);
                        _mm_store_ss(&t, sum);c[r * T + i][q * T + j] += t;
                    }
            }
        }
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < i; ++j)
                swap(b[i][j], b[j][i]);
}




// ���ǲ���
int main()
{
    /*
    //tesst1
    for(int n=10;n<maxN;){
        cout<<"���ڵľ����ģΪ��"<<n<<endl;

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                b[i][j] = a[i][j] = i + j;


        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        mul(n,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"�����㷨��" << (tail - head) * 1000.0 / freq << "ms" <<"    ";


        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        trans_mul(n,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"cache�Ż��㷨��" << (tail - head) * 1000.0 / freq << "ms" <<"    ";


        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        sse_mul(n,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"SSE�汾��" << (tail - head) * 1000.0 / freq << "ms" <<"    ";


        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        sse_tile(n,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"��Ƭ���ԣ�" << (tail - head) * 1000.0 / freq << "ms" <<endl;

        if (n<100)
            n+=10;
        else
            n+=100;

    }
*/



    //test2
    int time[4][10];
    for(int n=0;n<10;n++){
        cout<<"�����ģnΪ��"<<"500"<<endl;

        for (int i = 0; i < 500; i++)
            for (int j = 0; j < 500; j++)
                b[i][j] = a[i][j] = i + j;


        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        mul(500,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"�����㷨��" << (tail - head) * 1000.0 / freq << "ms" <<"    ";
        time[0][n] = (tail - head) * 1000.0 / freq;

        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        trans_mul(500,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"cache�Ż��㷨��" << (tail - head) * 1000.0 / freq << "ms" <<"    ";
        time[1][n] = (tail - head) * 1000.0 / freq;

        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        sse_mul(500,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"SSE�汾��" << (tail - head) * 1000.0 / freq << "ms" <<"    ";
        time[2][n] = (tail - head) * 1000.0 / freq;

        QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
        QueryPerformanceCounter((LARGE_INTEGER*)&head);
        sse_tile(500,a,b,c);
        QueryPerformanceCounter((LARGE_INTEGER*)&tail);
        cout <<"��Ƭ���ԣ�" << (tail - head) * 1000.0 / freq << "ms" <<endl;
        time[3][n] = (tail - head) * 1000.0 / freq;

    }
    int average[4];
    int sum=0;
    for(int j=0;j<4;j++)
    {
        for (int i=0;i<10;i++)
        {
            sum+=time[j][i];
        }
        average[j]=sum/10;
        sum=0;
    }
    cout<<"�����㷨ƽ����ʱ��"<<average[0]<<"ms"<<endl;
    cout<<"cache�Ż��㷨ƽ����ʱ��"<<average[1]<<"ms"<<endl;
    cout<<"SSE�㷨ƽ����ʱ��"<<average[2]<<"ms"<<endl;
    cout<<"��Ƭ�㷨ƽ����ʱ��"<<average[3]<<"ms"<<endl;




    return 1;

}





