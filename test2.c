#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 256

typedef struct {
    int data[MATRIX_SIZE][MATRIX_SIZE];
} Matrix;

void fillMatrix(Matrix* matrix) {
    int count = 1;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            matrix->data[i][j] = count++;
        }
    }
}

void multiplyMatrix(const Matrix* matrix1, const Matrix* matrix2, Matrix* result) {
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            result->data[i][j] = 0;
            for (int k = 0; k < MATRIX_SIZE; ++k) {
                result->data[i][j] += matrix1->data[i][k] * matrix2->data[k][j];
            }
        }
    }
}

DWORD WINAPI thread_function2(LPVOID lpParam) {
    double* execution_time = (double*)lpParam;

    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);

    Matrix matrix1, matrix2, result;

    fillMatrix(&matrix1);
    fillMatrix(&matrix2);

    multiplyMatrix(&matrix1, &matrix2, &result);

    QueryPerformanceCounter(&end_time);
    *execution_time = ((double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart) * 1000.0;

    return 0;
}

double run_test_2() {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    // Number of runs for averaging
    int num_runs = 200;

    double total_deviation = 0.0;

    for (int run = 0; run < num_runs; ++run) {
        // Array to store execution times for each thread
        double* execution_times = (double*)malloc(sys_info.dwNumberOfProcessors * sizeof(double));

        // Create threads
        HANDLE* threads = (HANDLE*)malloc(sys_info.dwNumberOfProcessors * sizeof(HANDLE));
        for (DWORD i = 0; i < sys_info.dwNumberOfProcessors; ++i) {
            execution_times[i] = 0.0;
            threads[i] = CreateThread(NULL, 0, thread_function2, &execution_times[i], 0, NULL);
            // Set thread priority to normal
            SetThreadPriority(threads[i], THREAD_PRIORITY_TIME_CRITICAL);
        }

        // Wait for all threads to finish
        WaitForMultipleObjects(sys_info.dwNumberOfProcessors, threads, TRUE, INFINITE);

        // Calculate the deviation between the best and worst thread execution times
        double best_execution_time = execution_times[0];
        double worst_execution_time = execution_times[0];

        for (DWORD i = 1; i < sys_info.dwNumberOfProcessors; ++i) {
            if (execution_times[i] > best_execution_time) {
                best_execution_time = execution_times[i];
            } else if (execution_times[i] < worst_execution_time) {
                worst_execution_time = execution_times[i];
            }
            CloseHandle(threads[i]);
        }

        // Free allocated memory
        free(execution_times);
        free(threads);

        // Calculate the percentage deviation
        double deviation = 0.0;
        if (best_execution_time > 0.0) {
            deviation = ((best_execution_time - worst_execution_time) / best_execution_time) * 100.0;
        }

        // Accumulate total deviation
        total_deviation += deviation;
    }

    // Average the deviation over multiple runs
    double average_deviation = total_deviation / num_runs;

    return average_deviation;
}