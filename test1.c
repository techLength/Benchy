#include <windows.h>
#include <stdio.h>

DWORD WINAPI thread_function1(LPVOID lpParam) {
    unsigned int* counter = (unsigned int*)lpParam;

    // Count up for 0.5 seconds in real-time
    ULONGLONG start_time = GetTickCount64();
    while (GetTickCount64() - start_time < 10) {
        (*counter)++;
    }

    return 0;
}

double run_test_1() {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    // Array to store counters for each thread
    unsigned int* counters = (unsigned int*)malloc(sys_info.dwNumberOfProcessors * sizeof(unsigned int));

    // Create threads
    HANDLE* threads = (HANDLE*)malloc(sys_info.dwNumberOfProcessors * sizeof(HANDLE));
    for (DWORD i = 0; i < sys_info.dwNumberOfProcessors; ++i) {
        counters[i] = 0;
        threads[i] = CreateThread(NULL, 0, thread_function1, &counters[i], 0, NULL);
        // Set thread priority to real-time
        SetThreadPriority(threads[i], THREAD_PRIORITY_TIME_CRITICAL);
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(sys_info.dwNumberOfProcessors, threads, TRUE, INFINITE);

    // Calculate total count and average result
    unsigned int total_count = 0;
    double average_result = 0.0;

    for (DWORD i = 0; i < sys_info.dwNumberOfProcessors; ++i) {
        total_count += counters[i];
        CloseHandle(threads[i]);
    }

    // Free allocated memory
    free(counters);
    free(threads);

    // Calculate the effective threads factor
    double effective_threads_count = 0.0;

    // Avoid division by zero
    if (sys_info.dwNumberOfProcessors > 0) {
        average_result = total_count / (double)sys_info.dwNumberOfProcessors;
        effective_threads_count = total_count / average_result;
    } 

    return effective_threads_count;
}