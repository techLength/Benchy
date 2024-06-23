#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <intrin.h>  // For __cpuid

// Function to get the CPU frequency in Hz
double get_cpu_frequency() {
    LARGE_INTEGER frequency;
    if (QueryPerformanceFrequency(&frequency)) {
        return (double)frequency.QuadPart;
    } else {
        printf("Unable to query performance frequency. Error code: %lu\n", GetLastError());
        return 0.0;
    }
}

DWORD WINAPI count_up_thread(LPVOID lpParam) {
    unsigned long long* total_count = (unsigned long long*)lpParam;

    ULONGLONG start_time = GetTickCount64();
    while (GetTickCount64() - start_time < 4000) {
        (*total_count)++;
    }

    return 0;
}

double run_test_4() {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    // Number of threads equals the number of processors
    DWORD num_threads = sys_info.dwNumberOfProcessors;

    // Array to store thread handles
    HANDLE* threads = (HANDLE*)malloc(num_threads * sizeof(HANDLE));
    if (threads == NULL) {
        printf("Memory allocation failed for main thread. Error code: %lu\n", GetLastError());
        return 0.0;
    }

    // Array to store total count for each thread
    unsigned long long* total_counts = (unsigned long long*)malloc(num_threads * sizeof(unsigned long long));
    if (total_counts == NULL) {
        printf("Memory allocation failed for main thread. Error code: %lu\n", GetLastError());
        free(threads);
        return 0.0;
    }

    // Create threads
    for (DWORD i = 0; i < num_threads; ++i) {
        total_counts[i] = 0;
        threads[i] = CreateThread(NULL, 0, count_up_thread, &total_counts[i], 0, NULL);
        if (threads[i] == NULL) {
            printf("Thread creation failed for thread %lu. Error code: %lu\n", i, GetLastError());
            // Cleanup
            for (DWORD j = 0; j < i; ++j) {
                CloseHandle(threads[j]);
            }
            free(threads);
            free(total_counts);
            return 0.0;
        }
        // Set thread priority to normal
        SetThreadPriority(threads[i], THREAD_PRIORITY_HIGHEST);
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(num_threads, threads, TRUE, INFINITE);

    // Calculate the total count
    unsigned long long total_count = 0;
    for (DWORD i = 0; i < num_threads; ++i) {
        total_count += total_counts[i];
        CloseHandle(threads[i]);
    }

    // Free allocated memory
    free(threads);
    free(total_counts);

    // Divide the output by the total thread count and the CPU frequency
    double cpu_frequency = 4 * get_cpu_frequency();
    return (double)total_count / (num_threads * cpu_frequency);
}