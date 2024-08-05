#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

typedef struct arguments{            //for passing the arguments into the runner function
    int min;
    int max;
    int i;
    int jump;
    FILE *fptr;
}arguments;

typedef struct vampireNumber{          //for storing the vampire no.s and their thread ID
    int vampireNo;
    int threadNo;
}vampNum;

int tot = 0;                           //for the total number of vampire numbers found
vampNum *mainArr;                      //for storing the vampire numbers and their thread ID
int upto=0;                            //for upadating the highest filled index of the mainArr

bool isVampireNumber(int num)
{
    int digitCountArr[10] = {0};
    int digitCount = 0;
    int digit;
    int temp = num;

    while (temp != 0)                      //to count the number of digits and how many times each digit is occurring in the number
    {
        digit = temp % 10;

        digitCount++;
        digitCountArr[digit]++;

        temp = temp/10;
    }

    for (int i = 1; i <= sqrt(num); i++)                    //for checking all the factors of num
    {
        if (num % i == 0)
        {
            int fac1 = i;
            int fac2 = num/i;
            int digitCount1 = 0, digitCount2 = 0;
            int digitCountArr_factors[10] = {0};

            while (fac1 != 0)                               //to count the number of digits and how many times each digit is occurring in the factor
            {
                digit = fac1 % 10;

                digitCount1++;
                digitCountArr_factors[digit]++;

                fac1 = fac1/10;
            }

            if (digitCount1 != digitCount / 2)
            {
                continue;                               //to next iteration/ pair of factors
            }

            while (fac2 != 0)                            //to count the number of digits and how many times each digit is occurring in the factor
            {
                digit = fac2 % 10;

                digitCount2++;
                digitCountArr_factors[digit]++;

                fac2 = fac2/10;
            }

            if (digitCount2 != digitCount / 2)
            {
                continue;                               //to next iteration/ pair of factors
            }

            int digitsNotSame = 0;
            for (int i = 0; i < 10; i++)
            {
                if (digitCountArr[i] != digitCountArr_factors[i])            //to check if the digits of the factors are the digits of num
                {
                    digitsNotSame = 1;
                }
            }
            if(digitsNotSame == 1)
            {
                continue;                               //to next iteration/ pair of factors
            }

            return true;  
        }
    }
    return false;                                           // if we cant find any pair satisfying the above conditions
}

void *runner(void *param)
{
    arguments *arg = (arguments *)param;                      //the arguments

    vampNum localArr[arg->max/arg->jump];                      //local array 

    int i = arg->min;
    int j=0;
    while(i <= arg->max)                                       //going till <=N
    {
        if(isVampireNumber(i))
        {
            localArr[j].vampireNo = i;
            localArr[j].threadNo = arg->i;                                      //storing in local array
            fprintf(arg->fptr, "%d : Found by Thread %d\n",i, arg->i);          //thread prints directly into the output file (if you want to print using the global array : comment this line out and un-comment lines 170 to 175)
            tot++;
            j++;
        }
        i = i + arg->jump;                                       //jumping by M
    }

    i=0;
    while(i<j)
    {
        mainArr[upto++] = localArr[i++];                           //transferring to global array
    }
    pthread_exit(0);
}

int main()
{
    int n,m;

    FILE *inFptr;
    inFptr = fopen("InFile.txt", "r");
    fscanf(inFptr, "%d", &n);
    fscanf(inFptr, "%d", &m);
    fclose(inFptr);

    mainArr = (vampNum *)malloc(n*sizeof(vampNum));             //allocating memory for the global array

    FILE *outFptr;
    outFptr = fopen("OutFile.txt", "w");
    fprintf(outFptr, "Vampire Numbers found:\n\n");
    
    pthread_t tid[m];                                   //array of threads
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    arguments argarr[m];                               //array of arguments
    for(int i=0; i<m; i++)
    {
        argarr[i].min = i + 1;                           //initial value
        argarr[i].max = n;                               //max allowed value
        argarr[i].i = i+1;                               //thread number
        argarr[i].jump = m;                              //jump by
        argarr[i].fptr = outFptr;                        //file pointer to printing file
    }

    for(int i=0; i<m; i++)
    {
        pthread_create(tid+i, &attr, runner, (void *)(argarr+i));
    }

    for(int i=0; i<m; i++)
    {
        pthread_join(tid[i], NULL);
    }

    // //if you want to print using the global array: comment out line 113 and un-comment lines 170 to 175

    // int i=0;
    // while(i < tot)
    // {  
    //     fprintf(outFptr, "%d : Found by Thread %d\n", mainArr[i].vampireNo, mainArr[i].threadNo);
    //     i++;
    // }

    fprintf(outFptr, "\nTotal Vampire numbers: %d\n", tot);
    fclose(outFptr);

    return 0;
}