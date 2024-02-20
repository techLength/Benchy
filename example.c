#include <stdio.h>
#include <windows.h>

typedef struct {
    double best_count;
    double worst_count;
    CRITICAL_SECTION lock;
} ThreadData;

DWORD WINAPI threadFunc(LPVOID lpParam);

double run_test_2() {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    ThreadData threadData;
    threadData.best_count = 1;
    threadData.worst_count = 1024 * 1024 * 1024;
    InitializeCriticalSection(&threadData.lock);

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numThreads = sysInfo.dwNumberOfProcessors;

    HANDLE* threads = (HANDLE*)malloc(numThreads * sizeof(HANDLE));
    if (threads == NULL) {
        fprintf(stderr, "Error allocating memory for threads\n");
        return -1;
    }

    for (int i = 0; i < numThreads; ++i) {
        threads[i] = CreateThread(NULL, 0, threadFunc, &threadData, 0, NULL);
        if (threads[i] == NULL) {
            fprintf(stderr, "Error creating thread %d\n", i);
            free(threads);
            DeleteCriticalSection(&threadData.lock);
            return -1;
        }
    }

    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    for (int i = 0; i < numThreads; ++i) {
        CloseHandle(threads[i]);
    }

    free(threads);
    DeleteCriticalSection(&threadData.lock);

    // Return the difference between the best and worst count
    return (double)(1 - (threadData.worst_count) / threadData.best_count) * 100;
}

DWORD WINAPI threadFunc(LPVOID lpParam) {
    ThreadData* threadData = (ThreadData*)lpParam;
    LARGE_INTEGER start_time, end_time;
    QueryPerformanceCounter(&start_time);

    // Count up from 1 for 0.01 seconds
    double count = 1.0;
    while (1) {
        QueryPerformanceCounter(&end_time);
        double elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / 1000000.0 / 1000.0; // Convert to seconds

        if (elapsed_time >= 0.01) {
            break;
        }

        count += 1.0;
    }

    // Update best and worst counts
    EnterCriticalSection(&threadData->lock);
    if (count > threadData->best_count) {
        threadData->best_count = count;
    }

    if (count < threadData->worst_count) {
        threadData->worst_count = count;
    }
    LeaveCriticalSection(&threadData->lock);

    return 0;
}

int main() {
    double result = run_test_2();
    printf("The difference between the best and worst count is: %.2f\n", result);

    return 0;
}