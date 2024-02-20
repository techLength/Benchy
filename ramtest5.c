#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEMORY_SIZE (4ULL * 1024 * 1024 * 1024) // 4GB

typedef struct {
    char* srcMemory;
    char* destMemory;
    size_t offset;
    size_t size;
} ThreadData;

DWORD WINAPI ram_test_5_thread(LPVOID param) {
    ThreadData* data = (ThreadData*)param;

    // Copy memory
    memcpy(data->destMemory + data->offset, data->srcMemory + data->offset, data->size);

    return 0;
}

double ram_test_5() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = sysInfo.dwNumberOfProcessors;

    // Allocate 4GB of memory
    char* srcMemoryBlock = (char*)malloc(MEMORY_SIZE);
    if (srcMemoryBlock == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return -1.0;
    }

    char* destMemoryBlock = (char*)malloc(MEMORY_SIZE);
    if (destMemoryBlock == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(srcMemoryBlock);
        return -1.0;
    }

    ThreadData* threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));
    if (threadData == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(srcMemoryBlock);
        free(destMemoryBlock);
        return -1.0;
    }

    // Initialize thread data
    size_t blockSize = MEMORY_SIZE / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].srcMemory = srcMemoryBlock;
        threadData[i].destMemory = destMemoryBlock;
        threadData[i].offset = i * blockSize;
        threadData[i].size = blockSize;
    }

    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threads == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(srcMemoryBlock);
        free(destMemoryBlock);
        free(threadData);
        return -1.0;
    }

    LARGE_INTEGER frequency, startTime, endTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);

    // Create threads to copy memory
    for (int i = 0; i < numThreads; ++i) {
        threads[i] = CreateThread(NULL, 0, ram_test_5_thread, &threadData[i], 0, NULL);
    }

    // Wait for threads to finish
    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    QueryPerformanceCounter(&endTime);

    // Free allocated resources
    free(threads);
    free(threadData);
    free(srcMemoryBlock);
    free(destMemoryBlock);

    // Calculate elapsed time in milliseconds
    double elapsedTime = ((double)(endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart);

    return 10 / elapsedTime;
}