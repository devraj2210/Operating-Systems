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
    int n=2048, k=16, rowInc = 16;
    ofstream out("inp.txt");
    out<<n<<" "<<k<<" "<<rowInc<<"\n";
    srand(0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int x = rand();
            out << x%10 << " ";
        }
        out << "\n";
    }
    return 0;
}