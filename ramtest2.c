#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_RUNS 1000
#define MEMORY_SIZE (1024 * 1024)

typedef struct {
    char* memory;
} ThreadData;

DWORD WINAPI ram_test_2_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;

    // Fill memory with random numbers
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        data->memory[i] = rand() % 256; // Filling with random numbers (0-255)
    }

    // Read from memory
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        char value = data->memory[i];
        // Do something with the read value if needed
    }

    // Copy memory
    char* copy = (char*)malloc(MEMORY_SIZE);
    if (copy != NULL) {
        memcpy(copy, data->memory, MEMORY_SIZE);
        free(copy);
    }

    // Overwrite memory
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        data->memory[i] = rand() % 256; // Overwriting with random numbers (0-255)
    }

    return 0;
}

double ram_test_2() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = sysInfo.dwNumberOfProcessors;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    double totalTime = 0.0;

    for (int run = 0; run < NUM_RUNS; ++run) {
        HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
        ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));

        LARGE_INTEGER startTime, endTime;
        QueryPerformanceCounter(&startTime);

        for (int i = 0; i < numThreads; ++i) {
            threadData[i].memory = (char*)malloc(MEMORY_SIZE);
            threads[i] = CreateThread(NULL, 0, ram_test_2_thread, &threadData[i], 0, NULL);

            SetThreadPriority(threads[i], THREAD_PRIORITY_HIGHEST);
        }

        WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

        QueryPerformanceCounter(&endTime);

        for (int i = 0; i < numThreads; ++i) {
            free(threadData[i].memory);
            CloseHandle(threads[i]);
        }

        free(threads);
        free(threadData);

        double elapsedMilliseconds = ((double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart) * 10.0;
        totalTime += elapsedMilliseconds;
    }

    double averageTime = totalTime / NUM_RUNS;

    // Return the performance metric
    return numThreads / averageTime;
}