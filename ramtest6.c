#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_RUNS 10000000
#define MEMORY_SIZE 1024 // 1 byte (single bit)

typedef struct {
    void* memory;
} ThreadData;

DWORD WINAPI ram_test_6_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;

    // Allocate and free memory
    for (int i = 0; i < NUM_RUNS; ++i) {
        data->memory = malloc(MEMORY_SIZE);
        if (data->memory != NULL) {
            free(data->memory);
        }
    }

    return 0;
}

double ram_test_6() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = sysInfo.dwNumberOfProcessors;

    ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    if (threadData == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return -1.0;
    }

    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threads == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(threadData);
        return -1.0;
    }

    LARGE_INTEGER frequency, startTime, endTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    // Create threads to allocate and free memory
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = CreateThread(NULL, 0, ram_test_6_thread, &threadData[i], 0, NULL);
    }

    // Wait for threads to finish
    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    QueryPerformanceCounter(&endTime);

    // Free allocated resources
    free(threads);
    free(threadData);

    // Calculate elapsed time in milliseconds
    double elapsedMilliseconds = ((double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart) * 1000.0;

    // Return the average latency across all threads
    return (1000 * 1000) * elapsedMilliseconds / numThreads / NUM_RUNS;
}