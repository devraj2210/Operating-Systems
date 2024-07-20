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
#include <unistd.h>
#include <semaphore.h>
#include <random>
#include <thread>

using namespace std;

// arguments to be passed to the threads
typedef struct arguments
{
    int thread_no;
} arguments;

// input variables
int nw, nr, kw, kr, u_CS, u_rem;

// to store wait times
double **wait_writers, **wait_readers;

// number of readers waiting for/running CS
int reader_count = 0;

// semaphores to implement the solution
sem_t read_sem, queue_sem, CS_sem;

// semaphore to protect the log entries
sem_t log_sem;

// files
ifstream in("inp_params.txt");
ofstream out_log("FairRW_log.txt");
ofstream out_time("FairRW_Average_time.txt");

// exponential distribution using random number generator
default_random_engine generator;
exponential_distribution<double> CS(1); // will be given actual parameter later in the code
auto CS_ptr = &CS;
exponential_distribution<double> rem(1); // will be given actual parameter later in the code
auto rem_ptr = &rem;

void *writer(void *param)
{
    // index of the thread in the thread array
    int id = ((arguments *)param)->thread_no;

    for (int i = 0; i < kw; i++)
    {
        // time of request
        auto reqTime = std::chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(reqTime);

        // print request time into logfile
        sem_wait(&log_sem);
        out_log << i + 1 << "th CS request by Writer Thread " << id + 1 << " at " << ctime(&time) << endl;
        sem_post(&log_sem);

        // entry section
        sem_wait(&queue_sem); // reserve entry section
        sem_wait(&CS_sem);    // reserve critical section
        sem_post(&queue_sem); // release entry section

        // time of entry into CS
        auto enterTime = std::chrono::system_clock::now();
        time = chrono::system_clock::to_time_t(enterTime);

        // store wait time into array
        auto wait_time = enterTime - reqTime;
        wait_writers[id][i] = chrono::duration<double, milli>(wait_time).count();

        // print entry time into logfile
        sem_wait(&log_sem);
        out_log << i + 1 << "th CS Entry by Writer Thread " << id + 1 << " at " << ctime(&time) << endl;
        sem_post(&log_sem);

        int randCSTime = CS(generator);
        this_thread::sleep_for(chrono::milliseconds(randCSTime)); // simulate a thread writing in CS

        // release CS
        sem_post(&CS_sem);

        // time of exit from CS
        auto exitTime = std::chrono::system_clock::now();
        time = chrono::system_clock::to_time_t(exitTime);

        // print exit time into logfile
        sem_wait(&log_sem);
        out_log << i + 1 << "th CS Exit by Writer Thread " << id + 1 << " at " << ctime(&time) << endl;
        sem_post(&log_sem);

        int randRemTime = rem(generator);
        this_thread::sleep_for(chrono::milliseconds(randRemTime)); // simulate a thread executing in Remainder Section
    }

    return NULL;
}

void *reader(void *param)
{
    // index of the thread in the thread array
    int id = ((arguments *)param)->thread_no;

    for (int i = 0; i < kr; i++)
    {
        // time of request
        auto reqTime = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(reqTime);

        // print request time into logfile
        sem_wait(&log_sem);
        out_log << i + 1 << "th CS request by Reader Thread " << id + 1 << " at " << ctime(&time) << endl;
        sem_post(&log_sem);

        sem_wait(&queue_sem); // Indicate a reader is trying to enter
        sem_wait(&read_sem);  // lock entry section
        reader_count++;       // report as a reader
        if (reader_count == 1)
            sem_wait(&CS_sem); // if first reader, lock the CS
        sem_post(&read_sem);   // release entry section for other readers
        sem_post(&queue_sem);

        // time of entry into CS
        auto enterTime = std::chrono::system_clock::now();
        time = chrono::system_clock::to_time_t(enterTime);

        // store wait time into array
        auto wait_time = enterTime - reqTime;
        wait_readers[id][i] = chrono::duration<double, milli>(wait_time).count();

        // print entry time into logfile
        sem_wait(&log_sem);
        out_log << i + 1 << "th CS Entry by Reader Thread " << id + 1 << " at " << ctime(&time) << endl;
        sem_post(&log_sem);

        int randCSTime = CS(generator);
        this_thread::sleep_for(chrono::milliseconds(randCSTime)); // simulate a thread reading from CS

        sem_wait(&read_sem); // reserve exit section
        reader_count--;      // indicate reader is leaving
        if (reader_count == 0)
            sem_post(&CS_sem); // if last reader, release the CS
        sem_post(&read_sem);

        // time of exit from CS
        auto exitTime = std::chrono::system_clock::now();
        time = chrono::system_clock::to_time_t(exitTime);

        // print exit time into logfile
        sem_wait(&log_sem);
        out_log << i + 1 << "th CS Exit by Reader Thread " << id + 1 << " at " << ctime(&time) << endl;
        sem_post(&log_sem);

        int randRemTime = rem(generator);
        this_thread::sleep_for(chrono::milliseconds(randRemTime)); // simulate a thread executing in Remainder Section
    }
    return NULL;
}

// returns maximum of two numbers
double max(double a, double b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

int main()
{
    // initialize the semaphores
    sem_init(&queue_sem, 0, 1);
    sem_init(&read_sem, 0, 1);
    sem_init(&log_sem, 0, 1);
    sem_init(&CS_sem, 0, 1);

    // read the inputs
    in >> nw >> nr >> kw >> kr >> u_CS >> u_rem;

    // allocate space for wait arrays
    wait_writers = new double *[nw];
    wait_readers = new double *[nr];

    for (int i = 0; i < nw; i++)
    {
        wait_writers[i] = new double[kw];
    }
    for (int i = 0; i < nr; i++)
    {
        wait_readers[i] = new double[kr];
    }

    // initialize wait times to zero
    for (int i = 0; i < nw; i++)
    {
        for (int j = 0; j < kw; j++)
        {
            wait_writers[i][j] = 0;
        }
    }
    for (int i = 0; i < nr; i++)
    {
        for (int j = 0; j < kr; j++)
        {
            wait_readers[i][j] = 0;
        }
    }

    // update the parameters
    exponential_distribution<double>::param_type new_lambda_CS((double)1 / u_CS);
    exponential_distribution<double>::param_type new_lambda_rem((double)1 / u_rem);

    CS_ptr->param(new_lambda_CS);
    rem_ptr->param(new_lambda_rem);

    pthread_t writers[nw]; // array of writer threads
    pthread_t readers[nr]; // array of reader threads
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // to store arguments
    arguments writer_no[nw];
    arguments reader_no[nr];

    for (int i = 0; i < nw; i++)
    {
        writer_no[i].thread_no = i;
        pthread_create(writers + i, &attr, writer, writer_no + i); // create the writer threads
    }
    for (int i = 0; i < nr; i++)
    {
        reader_no[i].thread_no = i;
        pthread_create(readers + i, &attr, reader, reader_no + i); // create the reader threads
    }

    for (int i = 0; i < nw; i++)
    {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < nr; i++)
    {
        pthread_join(readers[i], NULL);
    }

    double tot_wait_writers = 0, tot_wait_readers = 0;     // to calculate average time
    double worst_time_writers = 0, worst_time_readers = 0; // to calculate worst case time

    for (int i = 0; i < nw; i++)
    {
        for (int j = 0; j < kw; j++)
        {
            worst_time_writers = max(worst_time_writers, wait_writers[i][j]);
            tot_wait_writers += wait_writers[i][j];
        }
    }
    for (int i = 0; i < nr; i++)
    {
        for (int j = 0; j < kr; j++)
        {
            worst_time_readers = max(worst_time_readers, wait_readers[i][j]);
            tot_wait_readers += wait_readers[i][j];
        }
    }

    // output average times into the files
    out_time << "avg waiting time for writers: " << tot_wait_writers / (nw * kw) << "ms" << endl;
    out_time << "avg waiting time for readers: " << tot_wait_readers / (nr * kr) << "ms" << endl;

    // output worst case times into the terminal
    cout << "worst waiting time for writers: " << worst_time_writers << "ms" << endl;
    cout << "worst waiting time for readers: " << worst_time_readers << "ms" << endl;

    // deallocate space
    for (int i = 0; i < nw; i++)
    {
        delete[] wait_writers[i];
    }
    for (int i = 0; i < nr; i++)
    {
        delete[] wait_readers[i];
    }
    delete[] wait_writers;
    delete[] wait_readers;

    // close the files
    in.close();
    out_log.close();
    out_time.close();

    cout << "program finished succesfully" << endl;

    return 0;
}