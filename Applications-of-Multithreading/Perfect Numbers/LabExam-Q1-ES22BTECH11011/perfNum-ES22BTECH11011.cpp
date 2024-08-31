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

int N, K, tot = 0;
ofstream out("OutFile.txt");

typedef struct arguments // structure for arguments
{
    int start;
    int max;
    int jump;
    int thread_no;
} arguments;

bool isPerfect(int n)
{
    int sum = 0;
    for (int i = 1; i < n / 2 + 1; i++)
    {
        if (n % i == 0)
        {
            sum += i;
        }
    }
    if (sum == n)
        return 1;
    return 0;
}

void *runner(void *param)
{
    arguments *arg = (arguments *)param;

    int i = arg->start;
    while (i < arg->max) // going till <n+1
    {
        if (isPerfect(i))
        {
            out << i << ": Found by Thread " << arg->thread_no << "\n";
            tot++;
        }
        i = i + arg->jump; // jumping by k
    }
    pthread_exit(0);
}

int main()
{
    ifstream in("inp.txt"); // open the files
    in >> N >> K;           // input n and k form input file

    arguments argarr[K]; // array of arguments
    for (int i = 0; i < K; i++)
    {
        argarr[i].start = i+1;
        argarr[i].max = N+1;
        argarr[i].jump = K;
        argarr[i].thread_no = i;
    }

    pthread_t tid[K]; // array of threads
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    auto start = chrono::steady_clock::now(); // start counting time

    for (int i = 0; i < K; i++)
    {
        pthread_create(tid + i, &attr, runner, (void *)(argarr + i));
    }

    for (int i = 0; i < K; i++)
    {
        pthread_join(tid[i], NULL);
    }

    auto end = chrono::steady_clock::now(); // stop counting time
    auto time = end - start;
    cout << chrono::duration<double, milli>(time).count() << " ms" << endl; // output the time taken

    out << "Total Perfect Numbers: " << tot << "\n";

    in.close(); // close the files
    out.close();

    return 0;
}