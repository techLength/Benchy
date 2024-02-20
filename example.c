#include <stdio.h>
#include <stdlib.h>

#define MAX_ITERATIONS 6400

double ram_test_1() {
    printf("a\n");
    double maxRamInGB = 0.0;
    printf("b\n");
    void* memoryBlock = NULL;
    printf("c\n");
    size_t baseSize = 10 * 1024 * 1024;
    printf("d\n");
    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        printf("%i\n", i);
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
    printf("e\n");
    // Free the last allocated memory block before returning
    free(memoryBlock);
    printf("f\n");
    return maxRamInGB / 100.0;
}

void main() {
    double result = ram_test_1();
    printf("%.2f", result);
}