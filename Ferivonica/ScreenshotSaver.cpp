#include "ScreenshotSaver.h"
#include "lodepng.h"
#include <fstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <iostream>

ScreenshotSaver::ScreenshotSaver() {}

void ScreenshotSaver::CreateScreenshotFolder(const std::wstring& userName) {
    std::wstring folderPath = L"C:\\Users\\" + userName + L"\\AppData\\Roaming\\Screenshot";

    if (!CreateDirectory(folderPath.c_str(), NULL)) {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            std::wcout << "The Screenshot folder already exists." << std::endl;
        }
        else {
            std::wcerr << "Failed to create the Screenshot folder. Error code: " << error << std::endl;
        }
    }
    else {
        std::wcout << "The Screenshot folder was created successfully." << std::endl;
    }
}

void ScreenshotSaver::GetScreenshot(const std::wstring& userName) {
    int x1, y1, x2, y2, w, h;

    x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w = x2 - x1;
    h = y2 - y1;

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now);

    size_t bufferSize = 20;
    wchar_t* buffer = new wchar_t[bufferSize];

    size_t resultSize = 0;
    do {
        resultSize = std::wcsftime(buffer, bufferSize, L"%Y-%m-%d_%H-%M-%S", &timeinfo);
        if (resultSize == 0) {
            bufferSize *= 2;
            delete[] buffer;
            buffer = new wchar_t[bufferSize];
        }
    } while (resultSize == 0);

    std::wstring filename = GetValidFileName(userName) + L"_" + std::wstring(buffer);
    std::wstring pngFilePath = filename + L".png";

    SaveScreenshotAsPNG(pngFilePath, hBitmap, w, h);

    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
    delete[] buffer;
}

void ScreenshotSaver::SaveScreenshotAsPNG(const std::wstring& filePath, HBITMAP hBitmap, int width, int height) {
    std::vector<unsigned char> pngData;
    std::vector<unsigned char> bmpData(width * height * 4);
    BITMAPINFOHEADER bi{};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
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

    wchar_t userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (!GetUserNameW(userName, &userNameSize)) {
        std::cerr << "Failed to retrieve user name to save PNG files." << std::endl;
    }

    std::wstring fullFilePath = L"C:\\Users\\" + std::wstring(userName) + L"\\AppData\\Roaming\\Screenshot\\" + filePath;

    std::ofstream pngFile(fullFilePath, std::ios::binary);
    if (!pngFile.is_open()) {
        std::wcerr << L"Failed to open PNG file for writing: " << fullFilePath << std::endl;
        return;
    }
    pngFile.write(reinterpret_cast<const char*>(pngData.data()), pngData.size());

    std::wcout << L"PNG file saved: " << fullFilePath << std::endl;
}

void ScreenshotSaver::ScreenCaptureThread(const std::wstring& userName) {
    while (true) {
        GetScreenshot(userName);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

std::wstring ScreenshotSaver::GetValidFileName(const std::wstring& userName) {
    // Function to generate a valid filename, you can customize as needed
    return L"screenshot_" + userName;
}
