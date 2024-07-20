#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

using namespace std;

int n, k;
int **A; // 2-D arrays
int **A_trans;
int **A_sq;

typedef struct arguments // for passing the arguments into the runner function
{
     int start;
     int max;
     int jump;
} arguments;

int dot(int *a, int *b) // dot function for multiplying rows with columns
{
     int sum = 0;
     for (int i = 0; i < n; i++)
     {
          sum += a[i] * b[i];
     }
     return sum;
}

void *runner(void *param)
{
     arguments *arg = (arguments *)param;

     int i = arg->start;
     while (i < arg->max) // going till <n
     {
          for (int j = 0; j < n; j++)
          {
               A_sq[i][j] = dot(A[i], A_trans[j]);
          }
          i = i + arg->jump; // jumping by k
     }
     pthread_exit(0);
}

int main()
{
     ifstream in("inp.txt"); // open the files
     ofstream out("out_mixed.txt");
     in >> n >> k; // input n and k from input file

     A = new int *[n]; // allocate memory for the 2-D arrays
     A_sq = new int *[n];
     A_trans = new int *[n];
     for (int i = 0; i < n; i++)
     {
          A[i] = new int[n];
          A_sq[i] = new int[n];
          A_trans[i] = new int[n];
     }

     for (int i = 0; i < n; i++) // input the matrix into A
     {
          for (int j = 0; j < n; j++)
          {
               in >> A[i][j];
          }
     }
     for (int i = 0; i < n; i++)
     {
          for (int j = 0; j < n; j++)
          {
               A_trans[i][j] = A[j][i]; // compute transpose for ease of multiplying rows with columns
          }
     }

     arguments argarr[k]; // array of arguments
     for (int i = 0; i < k; i++)
     {
          argarr[i].start = i;
          argarr[i].max = n;
          argarr[i].jump = k;
     }

     pthread_t tid[k]; // array of threads
     pthread_attr_t attr;
     pthread_attr_init(&attr);

     auto start = chrono::steady_clock::now(); // start counting time

     for (int i = 0; i < k; i++)
     {
          pthread_create(tid + i, &attr, runner, (void *)(argarr + i));
     }

     for (int i = 0; i < k; i++)
     {
          pthread_join(tid[i], NULL);
     }

     auto end = chrono::steady_clock::now(); // stop counting time
     auto time = end - start;

     for (int i = 0; i < n; i++) // output squared matrix
     {
          for (int j = 0; j < n; j++)
          {
               out << A_sq[i][j] << "\t";
          }
          out << endl;
     }
     out << chrono::duration<double, milli>(time).count() << " ms" << endl; // output duration

     in.close(); // close files
     out.close();

     for (int i = 0; i < n; ++i) // deallocate memory
     {
          delete[] A[i];
          delete[] A_trans[i];
          delete[] A_sq[i];
     }
     delete[] A;
     delete[] A_trans;
     delete[] A_sq;

     return 0;
}