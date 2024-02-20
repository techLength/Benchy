#include <stdio.h>
#include <stdlib.h>

#define MAX_ITERATIONS 128002

double ram_test_1() {
    double maxRamInGB = 0.0;
    void* memoryBlock = NULL;
    size_t baseSize = 10 * 1024 * 1024;

    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        size_t blockSize = baseSize + i * baseSize; // Increase block size incrementally

        // Attempt to allocate memory
        memoryBlock = malloc(blockSize);

        if (memoryBlock != NULL) {
            // Allocation successful, update maxRamInMB
            maxRamInGB = i;
        } else {
            // Allocation failed, break out of the loop
            break;
        }

        free(memoryBlock);
    }

    // Free the last allocated memory block before returning
    free(memoryBlock);

    return maxRamInGB / 100.0;
}