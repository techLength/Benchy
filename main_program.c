// main_program.c
#include <windows.h>
#include <stdio.h>
#include "test_functions.h"

#define IDC_BACKGROUND 1
#define IDC_RESULT_TEXT_1 3
#define IDC_RESULT_TEXT_2 4
#define IDC_BUTTON_RUN_TEST 2
#define IDC_CPU_INFO_TEXT 5
#define IDC_GPU_INFO_TEXT 6
#define IDC_MOBO_INFO_TEXT 7
#define IDC_OS_INFO_TEXT 10
#define IDC_TIME_INFO_TEXT 11
#define IDC_OPEN_CPUZ 8
#define IDC_OPEN_GPUZ 9
#define IDC_OPEN_HWINFO 12
#define IDC_OPEN_UPDATE 13

HWND hBackground;
HWND hResultText1;
HWND hResultText2;
HWND cpuInfo;
HWND gpuInfo;
HWND moboInfo;
HWND osInfo;
HWND timeInfo;

// Function to update the text in the result box
void updateResultText(HWND hResultText, const char *prefix, int result) {
    char message[50];
    snprintf(message, sizeof(message), "%s%d", prefix, result);
    SetWindowText(hResultText, message);
}

// Function to open CPU-Z with optional parameters
void openCpuz(const char* parameters) {
    // Full path to the CPU-Z executable in the default installation location
    const char* cpuzPath = "C:\\Program Files\\CPUID\\CPU-Z\\cpuz.exe";

    ShellExecute(NULL, "open", cpuzPath, parameters, NULL, SW_SHOWNORMAL);
}

void openURL(const char* url) {
    ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void openHWInfo64(const char* parameters) {
    // Full path to the HWiNFO64 executable in the default installation location
    const char* hwInfo64Path = "C:\\Program Files\\HWiNFO64\\HWiNFO64.exe";

    ShellExecute(NULL, "open", hwInfo64Path, parameters, NULL, SW_SHOWNORMAL);
}

void openGpuz(const char* parameters) {
    // Full path to the GPU-Z executable in the default installation location
    const char* gpuzPath = "C:\\Program Files (x86)\\GPU-Z\\GPU-Z.exe";

    ShellExecute(NULL, "open", gpuzPath, parameters, NULL, SW_SHOWNORMAL);
}

// Callback function for the button click event
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_CREATE:
            // Load the background image and set it as the background of a static control
            hBackground = CreateWindow("STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP,
                                       0, 0, 800, 583, hwnd, (HMENU)IDC_BACKGROUND,
                                       GetModuleHandle(NULL), NULL);
            SendMessage(hBackground, STM_SETIMAGE, IMAGE_BITMAP,
                        (LPARAM)LoadImage(NULL, "background.bmp", IMAGE_BITMAP, 0, 0,
                                          LR_LOADFROMFILE | LR_CREATEDIBSECTION));

            CreateWindow("STATIC", "Version: 0.0.1", WS_CHILD | WS_VISIBLE | SS_CENTER,
                  684, 10, 100, 16, hwnd, NULL, NULL, NULL);

            // Create a static text control for displaying the result of test 1
            hResultText1 = CreateWindow("STATIC", "Test 1 Result: ", WS_CHILD | WS_VISIBLE,
                                        10, 80, 300, 20, hwnd, (HMENU)IDC_RESULT_TEXT_1,
                                        NULL, NULL);

            // Create a static text control for displaying the result of test 2
            hResultText2 = CreateWindow("STATIC", "Test 2 Result: ", WS_CHILD | WS_VISIBLE,
                                        10, 110, 300, 20, hwnd, (HMENU)IDC_RESULT_TEXT_2,
                                        NULL, NULL);

            // Create a static text control for displaying CPU information
            cpuInfo = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE,
                                        10, 391, 546, 17, hwnd, (HMENU)IDC_CPU_INFO_TEXT,
                                        NULL, NULL);   

            // Create a static text control for displaying GPU information
            gpuInfo = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE,
                                        10, 416, 546, 17, hwnd, (HMENU)IDC_GPU_INFO_TEXT,
                                        NULL, NULL);   

            // Create a static text control for displaying motherboard information
            moboInfo = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE,
                                        10, 441, 546, 17, hwnd, (HMENU)IDC_MOBO_INFO_TEXT,
                                        NULL, NULL);

            osInfo = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE,
                                        10, 466, 546, 17, hwnd, (HMENU)IDC_OS_INFO_TEXT,
                                        NULL, NULL);

            timeInfo = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE,
                                        10, 490, 546, 17, hwnd, (HMENU)IDC_TIME_INFO_TEXT,
                                        NULL, NULL);

            // Create a button to run tests
            CreateWindow("BUTTON", "Run Test", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                          10, 515, 100, 30, hwnd, (HMENU)IDC_BUTTON_RUN_TEST,
                          GetModuleHandle(NULL), NULL);

            // Create a button to open CPU-Z
            CreateWindow("BUTTON", "Open CPU-Z", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                          117, 515, 100, 30, hwnd, (HMENU)IDC_OPEN_CPUZ,
                          GetModuleHandle(NULL), NULL);

            // Create a button to open CPU-Z on the memory tab
            CreateWindow("BUTTON", "Open GPU-Z", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                          224, 515, 100, 30, hwnd, (HMENU)IDC_OPEN_GPUZ,
                          GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "Open HWiNFO64", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                          331, 515, 140, 30, hwnd, (HMENU)IDC_OPEN_HWINFO,
                          GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "Update", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                          478, 515, 78, 30, hwnd, (HMENU)IDC_OPEN_UPDATE,
                          GetModuleHandle(NULL), NULL);

            char* cpuModelName = get_cpu_model_name();
            char* gpuModelName = get_gpu_name();
            char* moboModelName = get_motherboard_name();
            char* osModelName = get_os_version();
            char* timeText = get_time();


            char cpuInfoText[256];
            snprintf(cpuInfoText, sizeof(cpuInfoText), "CPU Info: %s", cpuModelName);
            SetWindowText(cpuInfo, cpuInfoText);

            char gpuInfoText[256];
            snprintf(gpuInfoText, sizeof(gpuInfoText), "GPU Info: %s", gpuModelName);
            SetWindowText(gpuInfo, gpuInfoText);

            char moboInfoText[256];
            snprintf(moboInfoText, sizeof(moboInfoText), "Motherboard Info: %s", moboModelName);
            SetWindowText(moboInfo, moboInfoText);

            char osInfoText[256];
            snprintf(osInfoText, sizeof(osInfoText), "Operating System Info: %s", osModelName);
            SetWindowText(osInfo, osInfoText);

            char timeInfoText[256];
            snprintf(timeInfoText, sizeof(timeInfoText), "%s", timeText);
            SetWindowText(timeInfo, timeText);

            free(cpuModelName);
            free(osModelName);

            break;
        case WM_COMMAND:
            switch (LOWORD(w_param)) {
                case IDC_BUTTON_RUN_TEST:
                    // Call the first test function
                    int result1 = run_test_1();
                    updateResultText(hResultText1, "Custom Prefix for Test 1: ", result1);

                    // Call the second test function
                    int result2 = run_test_2();
                    updateResultText(hResultText2, "Custom Prefix for Test 2: ", result2);
                    break;
                case IDC_OPEN_CPUZ:
                    // Open CPU-Z
                    openCpuz("");
                    break;
                case IDC_OPEN_GPUZ:
                    // Open CPU-Z on the memory tab
                    openGpuz("");
                    break;
                case IDC_OPEN_HWINFO:
                    // Open CPU-Z on the memory tab
                    openHWInfo64("");
                    break;
                case IDC_OPEN_UPDATE:
                    // Open CPU-Z on the memory tab
                    openURL("https://github.com/techLength/Benchy/releases/tag/Production");
                    break;
            }
            // Add more cases for additional tests if needed
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, w_param, l_param);
    }
    return 0;
}

int main() {
    // Register the window class
    WNDCLASS window_class = {0};
    window_class.lpfnWndProc = WndProc;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    window_class.lpszClassName = "TestWindowClass";

    if (!RegisterClass(&window_class)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window
    HWND hwnd = CreateWindow("TestWindowClass", "Benchy", WS_OVERLAPPED | WS_SYSMENU,
                             CW_USEDEFAULT, CW_USEDEFAULT, 800, 583, NULL, NULL,
                             GetModuleHandle(NULL), NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}