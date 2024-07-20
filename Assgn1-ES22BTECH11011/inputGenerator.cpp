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
    int n=2048, k=8;
    ofstream out("inp.txt");
    out<<n<<" "<<k<<"\n";
    srand(0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            int x = rand();
            if ( x < 100)
            {
                out << x << " ";
            }
            else
            {
                j--;
            }
        }
        out << "\n";
    }
}