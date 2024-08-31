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
#include <atomic>

using namespace std;
std::atomic_int lock(0); // atomic variable lock

typedef struct arguments // structure for arguments
{
    int thread_no;
} arguments;

int n, k, rowInc, c = 0; // counter
int **A;                 // 2-D arrays
int **A_trans;
int **A_sq;
bool *waiting; // to store the wait status of threads

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
    int i = arg->thread_no;

    while (c < n)
    {
        waiting[i] = true;
        bool key = true;
        int expected = 0;
        while (waiting[i] && key) // waiting threads spin here
        {
            key = !lock.compare_exchange_strong(expected, 1);
            expected = 0;
        }
        waiting[i] = false;

        // critical section start
        int rowStart = c;
        c += rowInc;
        int rowEnd = c;
        // critical section end

        int j = (i + 1) % k;
        while ((j != i) && !waiting[j])
        {
            j = (j + 1) % k;
        }
        if (j == i)
        {
            lock.store(0); // release lock
        }
        else
        {
            waiting[j] = false; // release lock for thread j
        }

        for (int i = rowStart; i < rowEnd; i++)
        {
            for (int j = 0; j < n; j++)
            {
                A_sq[i][j] = dot(A[i], A_trans[j]);
            }
        }
    }
    pthread_exit(0);
}

int main()
{
    ifstream in("inp.txt"); // open the files
    ofstream out("out_CAS_bounded.txt");
    in >> n >> k >> rowInc; // input n and k form input file

    A = new int *[n]; // allocating space for the 2-D arrays
    A_sq = new int *[n];
    A_trans = new int *[n];
    waiting = new bool[k]; // allocate space for waiting list
    for (int i = 0; i < k; i++)
    {
        waiting[i] = false; // initialize to false
    }

    for (int i = 0; i < n; i++)
    {
        A[i] = new int[n];
        A_sq[i] = new int[n];
        A_trans[i] = new int[n];
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            in >> A[i][j]; // take the matrix from input file
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A_trans[i][j] = A[j][i]; // compute the transpose of A for ease of multiplying with dot product
        }
    }

    arguments argarr[k]; // array of arguments
    for (int i = 0; i < k; i++)
    {
        argarr[i].thread_no = i;
    }

    pthread_t tid[k]; // array of threads
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    auto start = chrono::steady_clock::now(); // start counting time

    for (int i = 0; i < k; i++)
    {
        pthread_create(tid + i, &attr, runner, argarr + i);
    }

    for (int i = 0; i < k; i++)
    {
        pthread_join(tid[i], NULL);
    }

    auto end = chrono::steady_clock::now(); // end counting time
    auto time = end - start;

    for (int i = 0; i < n; i++) // output the squared matrix into the output file
    {
        for (int j = 0; j < n; j++)
        {
            out << A_sq[i][j] << "\t";
        }
        out << endl;
    }
    out << chrono::duration<double, milli>(time).count() << " ms" << endl; // output the time taken

    in.close(); // close the files
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
    delete[] waiting;

    return 0;
}