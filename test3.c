#include <windows.h>
#include <stdint.h>
#include <stdio.h>

#define MEMORY_READ_SIZE 1024 * 1024 * 1024  // Using 32 bytes
#define TARGET_READS_PER_THREAD 1024 * 1024 * 64  // Adjust based on your needs

// Function to read memory
DWORD WINAPI read_memory_thread(LPVOID lpParam) {
    size_t* total_reads = (size_t*)lpParam;

    // Allocate a separate buffer for each thread
    char* buffer = (char*)malloc(MEMORY_READ_SIZE);
    if (buffer == NULL) {
        printf("Memory allocation failed for thread %lu. Error code: %lu\n", GetCurrentThreadId(), GetLastError());
        return 1;
    }

    memset(buffer, 0, MEMORY_READ_SIZE);

    // Count reads until the target is reached
    while (*total_reads < TARGET_READS_PER_THREAD) {
        // Read memory
        char result = buffer[0];  // Each thread reads from its own buffer

        // Perform some operation with the result to prevent compiler optimization
        buffer[0] = result + 1;

        // Increment the total reads
        (*total_reads)++;
    }

    free(buffer);

    return 0;
}

double run_test_3_avx2() {
    LARGE_INTEGER start_time, end_time, frequency;
    QueryPerformanceFrequency(&frequency);

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

    // Array to store total reads for each thread
    size_t* total_reads = (size_t*)malloc(num_threads * sizeof(size_t));
    if (total_reads == NULL) {
        printf("Memory allocation failed for main thread. Error code: %lu\n", GetLastError());
        free(threads);
        return 0.0;
    }

    QueryPerformanceCounter(&start_time);  // Record start time

    // Create threads
    for (DWORD i = 0; i < num_threads; ++i) {
        total_reads[i] = 0;
        threads[i] = CreateThread(NULL, 0, read_memory_thread, &total_reads[i], 0, NULL);
        if (threads[i] == NULL) {
            printf("Thread creation failed for thread %lu. Error code: %lu\n", i, GetLastError());
            // Cleanup
            for (DWORD j = 0; j < i; ++j) {
                CloseHandle(threads[j]);
            }
            free(threads);
            free(total_reads);
            return 0.0;
        }
        // Set thread priority to normal
        SetThreadPriority(threads[i], THREAD_PRIORITY_TIME_CRITICAL);
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(num_threads, threads, TRUE, INFINITE);

    QueryPerformanceCounter(&end_time);  // Record end time

    // Calculate the total time taken by each thread
    double elapsed_time = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;
    double inverse_time = 10 * num_threads / elapsed_time;

    // Free allocated memory
    for (DWORD i = 0; i < num_threads; ++i) {
        CloseHandle(threads[i]);
    }
    free(threads);
    free(total_reads);

    // Return the inverse of the time taken multiplied by the number of threads
    return inverse_time;
}