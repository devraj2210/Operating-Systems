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

int main()
{
    int n=1024, k=64, c=16, bt = 32;
    ofstream out("inp.txt");
    out<<n<<" "<<k<<" "<<c<<" "<<bt<<"\n";
    srand(0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int x = rand();
            out << x%1000<< " ";
        }
        out << "\n";
    }
}