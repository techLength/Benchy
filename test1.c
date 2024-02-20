#include <windows.h>
#include <stdio.h>
#include <immintrin.h>

#define NUM_RUNS 1024 * 1024 * 2
#define ARRAY_SIZE 1024 * 256 * 2
#define ALIGNMENT 32

// Thread data structure
typedef struct {
    int* restrict array;
    int start;
    int end;
    double elapsedTime; // To store thread execution time
} ThreadData;

// Thread function for individual test
DWORD WINAPI individual_test_thread(LPVOID param) {
    ThreadData* restrict data = (ThreadData*)param;

    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    __m256i increment = _mm256_set1_epi32(1);

    for (int run = 0; run < NUM_RUNS; ++run) {
        int i = data->start;
        for (; i < data->end - 8; i += 8) {
            __m256i vec = _mm256_load_si256((__m256i*)&data->array[i]);
            vec = _mm256_add_epi32(vec, increment);
            _mm256_store_si256((__m256i*)&data->array[i], vec);
        }

        // Handle remaining elements (less than 8)
        for (; i < data->end; ++i) {
            data->array[i] += 1;
        }
    }

    QueryPerformanceCounter(&end);
    data->elapsedTime = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

    printf("Individual Thread %d finished in %.6f seconds.\n", GetCurrentThreadId(), data->elapsedTime);

    return 0;
}

// Function to perform individual test on each thread and return the metric
double individual_test(int* restrict array, int numThreads) {
    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    ThreadData* restrict threadData = (ThreadData*)malloc(numThreads * sizeof(ThreadData));

    // Align the array
    int* alignedArray = _aligned_malloc(ARRAY_SIZE * sizeof(int), ALIGNMENT);
    if (alignedArray == NULL) {
        printf("Memory allocation failed.\n");
        exit(1); // Terminate the program if memory allocation fails
    }

    // Initialize aligned array
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        alignedArray[i] = array[i];
    }

    // Divide the array among threads
    int chunkSize = ARRAY_SIZE / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].array = alignedArray;
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? ARRAY_SIZE : (i + 1) * chunkSize;

        // Create thread
        threads[i] = CreateThread(NULL, 0, individual_test_thread, &threadData[i], 0, NULL);
        if (threads[i] == NULL) {
            printf("Individual Thread %d creation failed. Error code: %lu\n", i, GetLastError());
            exit(1); // Terminate the program if thread creation fails
        }

        // Set thread priority to normal
        SetThreadPriority(threads[i], THREAD_PRIORITY_NORMAL);

        // Disable frequency scaling (example for Windows)
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    // Calculate the average time per thread
    double avgTimePerThread = 0.0;
    double longestTime = 0.0;

    for (int i = 0; i < numThreads; ++i) {
        avgTimePerThread += threadData[i].elapsedTime;

        if (threadData[i].elapsedTime > longestTime) {
            longestTime = threadData[i].elapsedTime;
        }

        CloseHandle(threads[i]);
    }

    // Free aligned memory
    _aligned_free(alignedArray);

    free(threads);
    free(threadData);

    // Return the calculated metric
    return avgTimePerThread / longestTime;
}

double cpustress() {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    int numThreads = sys_info.dwNumberOfProcessors;

    int* restrict array = (int*)malloc(ARRAY_SIZE * sizeof(int));
    if (array == NULL) {
        return 1; // Terminate the program if memory allocation fails
    }

    // Initialize array
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        array[i] = i;
    }

    // Individual Test on each thread and get the metric
    double metric = individual_test(array, numThreads);

    free(array);

    return metric;
}
