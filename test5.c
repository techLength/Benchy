#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ARRAY_SIZE 16
#define MATRIX_SIZE 16
#define NUM_ITERATIONS 750000

// Define Octonion structure
typedef struct {
    double data[8];
} Octonion;

// Function to add two Octonions
Octonion addOctonions(Octonion a, Octonion b) {
    Octonion result;
    for (int i = 0; i < 8; ++i) {
        result.data[i] = a.data[i] + b.data[i];
    }
    return result;
}

// Function to create a 16x16 array filled with Octonions
void fillArray(Octonion (*array)[MATRIX_SIZE][MATRIX_SIZE]) {
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            // Filling Octonions with random values for demonstration
            for (int k = 0; k < 8; ++k) {
                (*array)[i][j].data[k] = rand() % 100;
            }
        }
    }
}

// Thread function to add arrays
DWORD WINAPI addArraysThread(LPVOID param) {
    Octonion (*arrays)[MATRIX_SIZE][MATRIX_SIZE] = (Octonion(*)[MATRIX_SIZE][MATRIX_SIZE])param;

    // Adding 16 arrays 1000 times
    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        Octonion result = {0};
        for (int i = 0; i < MATRIX_SIZE; ++i) {
            for (int j = 0; j < MATRIX_SIZE; ++j) {
                result = addOctonions(result, (*arrays)[i][j]);
            }
        }
    }

    return 0;
}

double run_test_5() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = sysInfo.dwNumberOfProcessors;

    // Initialize arrays
    Octonion arrays[ARRAY_SIZE][MATRIX_SIZE][MATRIX_SIZE];
    fillArray(arrays);  // Pass the 3D array directly

    // Initialize threads
    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    DWORD* threadIds = (DWORD*)malloc(numThreads * sizeof(DWORD));

    // Measure performance start time
    LARGE_INTEGER startTime, endTime, frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    // Create threads
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = CreateThread(NULL, 0, addArraysThread, (LPVOID)arrays, 0, &threadIds[i]);
        SetThreadPriority(threads[i], THREAD_PRIORITY_TIME_CRITICAL);
    }

    // Wait for threads to finish
    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    // Measure performance end time
    QueryPerformanceCounter(&endTime);

    // Calculate elapsed time
    double elapsedSeconds = (double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart;

    // Print the elapsed time
    printf("Elapsed Time: %.2f seconds\n", 100 / elapsedSeconds);

    // Close thread handles
    for (int i = 0; i < numThreads; ++i) {
        CloseHandle(threads[i]);
    }

    free(threads);
    free(threadIds);

    return (double)100 / elapsedSeconds;
}