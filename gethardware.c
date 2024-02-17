//gethardware.c
#include <intrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpuid.h>
#include <windows.h>

char* get_cpu_model_name() {
    unsigned int eax, ebx, ecx, edx;
    char model_name[49];

    // Get CPU model name
    __cpuid(0x80000002, eax, ebx, ecx, edx);
    memcpy(model_name, &eax, 4);
    memcpy(model_name + 4, &ebx, 4);
    memcpy(model_name + 8, &ecx, 4);
    memcpy(model_name + 12, &edx, 4);

    __cpuid(0x80000003, eax, ebx, ecx, edx);
    memcpy(model_name + 16, &eax, 4);
    memcpy(model_name + 20, &ebx, 4);
    memcpy(model_name + 24, &ecx, 4);
    memcpy(model_name + 28, &edx, 4);

    __cpuid(0x80000004, eax, ebx, ecx, edx);
    memcpy(model_name + 32, &eax, 4);
    memcpy(model_name + 36, &ebx, 4);
    memcpy(model_name + 40, &ecx, 4);
    memcpy(model_name + 44, &edx, 4);
    model_name[48] = '\0';

    // Allocate memory for the result string
    char* result = (char*)malloc(sizeof(char) * 49);

    // Check if memory allocation was successful
    if (result != NULL) {
        // Copy the model name to the allocated memory
        snprintf(result, 49, "%s", model_name);
    }

    return result;
}

char* get_gpu_name() {
    DISPLAY_DEVICE displayDevice = {0};
    displayDevice.cb = sizeof(displayDevice);

    // Enumerate the display devices
    if (EnumDisplayDevices(NULL, 0, &displayDevice, 0)) {
        // You may want to allocate a buffer dynamically based on the actual length of the GPU name
        // For simplicity, a static buffer is used here.
        static char gpuName[128];
        snprintf(gpuName, sizeof(gpuName), "%s", displayDevice.DeviceString);
        return gpuName;
    }

    return NULL;
}

char* get_motherboard_name() {
    HKEY hKey;
    char motherboardName[128];
    DWORD size = sizeof(motherboardName);

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueEx(hKey, "BaseBoardProduct", NULL, NULL, (LPBYTE)motherboardName, &size) == ERROR_SUCCESS) {
            static char result[128];
            snprintf(result, sizeof(result), "%s", motherboardName);
            RegCloseKey(hKey);
            return result;
        }
        RegCloseKey(hKey);
    }

    return NULL;
}

char* get_os_version() {
    HKEY hKey;
    DWORD dwType;
    DWORD dwSize;

    // Open the registry key
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char buffer[256];  // A buffer to hold the concatenated string
        char* result = NULL;

        // Read product name
        dwSize = sizeof(buffer);
        if (RegQueryValueEx(hKey, "ProductName", NULL, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
            result = strdup(buffer);
        }

        // Read display version
        dwSize = sizeof(buffer);
        if (RegQueryValueEx(hKey, "DisplayVersion", NULL, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
            char* temp = result;
            result = (char*)malloc(strlen(temp) + strlen(buffer) + 2);  // +2 for separating space and null terminator
            snprintf(result, strlen(temp) + strlen(buffer) + 2, "%s %s", temp, buffer);
            free(temp);
        }

        // Read build number
        dwSize = sizeof(buffer);
        if (RegQueryValueEx(hKey, "CurrentBuildNumber", NULL, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
            char* temp = result;
            result = (char*)malloc(strlen(temp) + strlen(buffer) + 2);  // +2 for separating space and null terminator
            snprintf(result, strlen(temp) + strlen(buffer) + 2, "%s %s", temp, buffer);
            free(temp);
        }

        // Close the registry key
        RegCloseKey(hKey);
        return result;
    }

    return NULL;
}

char* get_time() {
    // Get current UTC time
    SYSTEMTIME st;
    GetSystemTime(&st);

    // Map month number to month name
    const char* monthNames[] = { "", "January", "February", "March", "April", "May", "June",
                                 "July", "August", "September", "October", "November", "December" };

    // Format the time info
    char buffer[120];  // Increased buffer size to accommodate epoch time
    snprintf(buffer, sizeof(buffer), "Time Info: %02d:%02d %s %d, %d                                            Uptime: %lld",
             st.wHour, st.wMinute, monthNames[st.wMonth], st.wDay, st.wYear,
             (long long)GetTickCount64());

    // Copy the result to a new string
    char* result = (char*)malloc(strlen(buffer) + 1);  // +1 for null terminator
    strcpy(result, buffer);

    return result;
}
