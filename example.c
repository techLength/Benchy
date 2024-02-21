#include <stdio.h>
#include <Windows.h>

double getFreePhysicalMemoryInGB() {
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    if (GlobalMemoryStatusEx(&memoryStatus)) {
        // Get the amount of available physical memory in gigabytes
        double availablePhysicalMemoryGB = (double)memoryStatus.ullAvailPhys / (1024 * 1024 * 1024);
        return availablePhysicalMemoryGB;
    } else {
        printf("Failed to retrieve memory status. Error code: %lu\n", GetLastError());
        return -1.0; // Return a negative value to indicate an error
    }
}

int main() {
    double freeMemory = getFreePhysicalMemoryInGB();

    if (freeMemory >= 0.0) {
        printf("Available physical memory: %.2f GB\n", freeMemory);
    } else {
        printf("Failed to retrieve available physical memory.\n");
    }

    return 0;
}