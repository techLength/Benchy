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
#define IDC_RESULT_TEXT_3 14
#define IDC_RESULT_TEXT_4 15
#define IDC_RESULT_TEXT_5 16
#define IDC_RESULT_TEXT_S 17
#define IDC_RESULT_MEM_S 18
#define IDC_RESULT_MEM_6 19
#define IDC_RESULT_MEM_1 20
#define IDC_RESULT_MEM_2 21
#define IDC_RESULT_MEM_3 22
#define IDC_RESULT_MEM_4 23
#define IDC_RESULT_MEM_5 24
#define ID_EDIT 18
#define IDC_RESULT_TOTAL 25

HWND hBackground;
HWND hResultText1;
HWND hResultText2;
HWND hResultText3;
HWND hResultText4;
HWND hResultText5;
HWND hResultTextS;
HWND hResultMem1;
HWND hResultMem2;
HWND hResultMem3;
HWND hResultMem4;
HWND hResultMem5;
HWND hResultMem6;
HWND hResultMemS;
HWND cpuInfo;
HWND gpuInfo;
HWND moboInfo;
HWND osInfo;
HWND timeInfo;
HWND hResultTotal;

// Function to update the text in the result box
void updateResultText(HWND hResultText, const char *prefix, double result) {
    char message[50];
    snprintf(message, sizeof(message), "%s%.2f", prefix, result);
    SetWindowText(hResultText, message);
}

void updateText(HWND hResultText, const char *prefix) {
    char message[50];
    snprintf(message, sizeof(message), "%s", prefix);
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

void ReadUsernameFromRegistry(char* buffer, DWORD bufferSize) {
    HKEY hKey;
    DWORD dataSize = bufferSize;

    // Open or create the registry key
    if (RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\techLength\\Benchy", 0, NULL, 0, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        // Read the username value
        if (RegQueryValueEx(hKey, "Username", NULL, NULL, (BYTE*)buffer, &dataSize) != ERROR_SUCCESS) {
            // Set a default value if the key is not found or any error occurs
            strcpy_s(buffer, bufferSize, "DefaultUsername");

            // Write the default value to the registry
            RegSetValueEx(hKey, "Username", 0, REG_SZ, (BYTE*)buffer, (DWORD)strlen(buffer) + 1);
        }

        // Close the registry key
        RegCloseKey(hKey);
    } else {
        // Default value if the key cannot be opened or created
        strcpy_s(buffer, bufferSize, "DefaultUsername");
    }
}

void WriteUsernameToRegistry(const char* username) {
    HKEY hKey;
    // Open or create the registry key
    if (RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\techLength\\Benchy", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        // Write the username value
        RegSetValueEx(hKey, "Username", 0, REG_SZ, (BYTE*)username, strlen(username) + 1);

        // Close the registry key
        RegCloseKey(hKey);
    } else {
        MessageBox(NULL, "Failed to write to the registry!", "Error", MB_ICONERROR | MB_OK);
    }
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
                        (LPARAM)LoadImage(NULL, "resources\\background.bmp", IMAGE_BITMAP, 0, 0,
                                          LR_LOADFROMFILE | LR_CREATEDIBSECTION));

            CreateWindow("STATIC", "Version: 1.2.3", WS_CHILD | WS_VISIBLE | SS_CENTER,
                  684, 10, 100, 16, hwnd, NULL, NULL, NULL);


            char usernameBuffer[256];
            ReadUsernameFromRegistry(usernameBuffer, sizeof(usernameBuffer));

            HWND hEdit = CreateWindowEx(
                0,
                "EDIT",
                usernameBuffer,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                10,  // X position
                10,  // Y position
                200, // Width
                20, // Height
                hwnd,
                (HMENU)ID_EDIT,
                GetModuleHandle(NULL),
                NULL
            );

            // Create a static text control for displaying the result of test 1
            hResultText1 = CreateWindow("STATIC", "Effective Threads: ", WS_CHILD | WS_VISIBLE,
                                        10, 80, 252, 20, hwnd, (HMENU)IDC_RESULT_TEXT_1,
                                        NULL, NULL);

            // Create a static text control for displaying the result of test 2
            hResultText2 = CreateWindow("STATIC", "Performance Deviation: ", WS_CHILD | WS_VISIBLE,
                                        10, 110, 252, 20, hwnd, (HMENU)IDC_RESULT_TEXT_2,
                                        NULL, NULL);

                                        // Create a static text control for displaying the result of test 3
            hResultText3 = CreateWindow("STATIC", "Multithreaded Points: ", WS_CHILD | WS_VISIBLE,
                                        10, 140, 252, 20, hwnd, (HMENU)IDC_RESULT_TEXT_3,
                                        NULL, NULL);

                          // Create a static text control for displaying the result of test 4
            hResultText4 = CreateWindow("STATIC", "Single Thread Points: ", WS_CHILD | WS_VISIBLE,
                                        10, 170, 252, 20, hwnd, (HMENU)IDC_RESULT_TEXT_4,
                                        NULL, NULL);

                                        // Create a static text control for displaying the result of test 4
            hResultText5 = CreateWindow("STATIC", "Array Points: ", WS_CHILD | WS_VISIBLE,
                                        10, 200, 252, 20, hwnd, (HMENU)IDC_RESULT_TEXT_5,
                                        NULL, NULL);

                                        // Create a static text control for displaying the result of test 4
            hResultTextS = CreateWindow("STATIC", "CPU Score: ", WS_CHILD | WS_VISIBLE,
                                        10, 260, 252, 20, hwnd, (HMENU)IDC_RESULT_TEXT_S,
                                        NULL, NULL);




            hResultMem1 = CreateWindow("STATIC", "Effective Memory: ", WS_CHILD | WS_VISIBLE,
                                        270, 80, 253, 20, hwnd, (HMENU)IDC_RESULT_MEM_1,
                                        NULL, NULL);
            hResultMem2 = CreateWindow("STATIC", "Cache Points: ", WS_CHILD | WS_VISIBLE,
                                        270, 110, 253, 20, hwnd, (HMENU)IDC_RESULT_MEM_2,
                                        NULL, NULL);
            hResultMem3 = CreateWindow("STATIC", "Read Points: ", WS_CHILD | WS_VISIBLE,
                                        270, 140, 253, 20, hwnd, (HMENU)IDC_RESULT_MEM_3,
                                        NULL, NULL);
            hResultMem4 = CreateWindow("STATIC", "Write Points: ", WS_CHILD | WS_VISIBLE,
                                        270, 170, 253, 20, hwnd, (HMENU)IDC_RESULT_MEM_4,
                                        NULL, NULL);
            hResultMem5 = CreateWindow("STATIC", "Copy Points: ", WS_CHILD | WS_VISIBLE,
                                        270, 200, 253, 20, hwnd, (HMENU)IDC_RESULT_MEM_5,
                                        NULL, NULL);
            hResultMem6 = CreateWindow("STATIC", "Latency: ", WS_CHILD | WS_VISIBLE,
                                        270, 230, 253, 20, hwnd, (HMENU)IDC_RESULT_MEM_6,
                                        NULL, NULL);
            hResultMemS = CreateWindow("STATIC", "Memory Score: ", WS_CHILD | WS_VISIBLE,
                                        270, 290, 253, 20, hwnd, (HMENU)IDC_RESULT_MEM_S,
                                        NULL, NULL);


            hResultTotal = CreateWindow("STATIC", "Total Score: ", WS_CHILD | WS_VISIBLE,
                                        574, 353, 210, 20, hwnd, (HMENU)IDC_RESULT_TOTAL,
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
                    updateText(hResultText1, "Effective Threads: Testing");
                                        Sleep(500);

                    double cpuresult1 = cpustress();
                    updateResultText(hResultText1, "Effective Threads: ", cpuresult1);
                    Sleep(500);

                    



                    // Call the second test function
                    updateText(hResultText2, "Performance Deviation: Testing");
                                        Sleep(500);

                    double cpuresult2 = run_test_2();
                    updateResultText(hResultText2, "Performance Deviation: ", cpuresult2);




                    updateText(hResultText3, "Multithread Points: Testing");
                                        Sleep(500);

                    double cpuresult3 = run_test_3_avx2();
                    updateResultText(hResultText3, "Multithreaded Points: ", cpuresult3);


                    updateText(hResultText4, "Single Thread Points: Testing");
                                        Sleep(500);

                    double cpuresult4 = run_test_4();
                    updateResultText(hResultText4, "Single Thread Points: ", cpuresult4);

                    updateText(hResultText5, "Array Points: Testing");
                                        Sleep(500);

                    double cpuresult5 = run_test_5();
                    updateResultText(hResultText5, "Array Points: ", cpuresult5);

                    updateText(hResultTextS, "CPU Score: Testing");
                                        Sleep(500);

                    double cpuScore = cpuresult5 + cpuresult4 + cpuresult3 - cpuresult2 - cpuresult1;
                    updateResultText(hResultTextS, "CPU Score: ", cpuScore);




Sleep(500);
                    updateText(hResultMem1, "Effective Memory: Testing");
                    Sleep(500);
                    double memResult1 = ram_test_1();
                    updateResultText(hResultMem1, "Effective Memory: ", memResult1);

                    updateText(hResultMem2, "Cache Points: Testing");
                    Sleep(500);
                    double memResult2 = ram_test_2();
                    updateResultText(hResultMem2, "Cache Points: ", memResult2);

                    updateText(hResultMem3, "Read Points: Testing");
                    Sleep(500);
                    double memResult3 = ram_test_3();
                    updateResultText(hResultMem3, "Read Points: ", memResult3);

                    updateText(hResultMem4, "Write Points: Testing");
                    Sleep(500);
                    double memResult4 = ram_test_4();
                    updateResultText(hResultMem4, "Write Points: ", memResult4);

                    updateText(hResultMem5, "Copy Points: Testing");
                    Sleep(500);
                    double memResult5 = ram_test_5();
                    updateResultText(hResultMem5, "Copy Points: ", memResult5);

                    updateText(hResultMem6, "Latency: Testing");
                    Sleep(500);
                    double memResult6 = ram_test_6();
                    updateResultText(hResultMem6, "Latency: ", memResult6);

                   updateText(hResultMemS, "Memory Points: Testing");
                  Sleep(500);
                    double memScore = memResult1 + memResult2 + memResult3 + memResult4 + memResult5 - memResult6;
                  updateResultText(hResultMemS, "Memory Score: ", memScore);

                    updateText(hResultTotal, "Total Points: Testing");
                    Sleep(500);
                    double totalScore = cpuScore + memScore;
                    updateResultText(hResultTotal, "Total Score: ", totalScore);

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
                    openURL("https://github.com/techLength/Benchy/releases");
                    break;
            }

            if (LOWORD(w_param) == ID_EDIT && HIWORD(w_param) == EN_CHANGE) {
                // Edit control content has changed
                char usernameBuffer[256];
                GetWindowText(GetDlgItem(hwnd, ID_EDIT), usernameBuffer, sizeof(usernameBuffer));

                // Write the updated username to the registry
                WriteUsernameToRegistry(usernameBuffer);
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