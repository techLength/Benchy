#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEMORY_SIZE (4ULL * 1024 * 1024 * 1024) // 4GB

typedef struct {
    char* memory;
    size_t offset;
    size_t size;
} ThreadData;

DWORD WINAPI ram_test_4_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;

    // Fill memory with zeros
    for (size_t i = 0; i < data->size; ++i) {
        data->memory[data->offset + i] = 0;
    }

    return 0;
}

double ram_test_4() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = sysInfo.dwNumberOfProcessors;

    // Allocate 4GB of memory
    char* memoryBlock = (char*)malloc(MEMORY_SIZE);
    if (memoryBlock == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return -1.0;
    }

    ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    if (threadData == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(memoryBlock);
        return -1.0;
    }

    // Initialize thread data
    size_t blockSize = MEMORY_SIZE / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].memory = memoryBlock;
        threadData[i].offset = i * blockSize;
        threadData[i].size = blockSize;
    }

    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threads == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(memoryBlock);
        free(threadData);
        return -1.0;
    }

    LARGE_INTEGER frequency, startTime, endTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    // Create threads to fill memory with zeros
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = CreateThread(NULL, 0, ram_test_4_thread, &threadData[i], 0, NULL);

        SetThreadPriority(threads[i], THREAD_PRIORITY_HIGHEST);
    }

    // Wait for threads to finish
    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    QueryPerformanceCounter(&endTime);

    // Free allocated resources
    free(threads);
    free(threadData);
    free(memoryBlock);

    // Calculate elapsed time in milliseconds
    double elapsedTime = ((double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart);

    return 10 / elapsedTime;
}