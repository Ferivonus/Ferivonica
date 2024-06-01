#ifndef SCREENSHOT_SAVER_H
#define SCREENSHOT_SAVER_H

#include <Windows.h>
#include <string>
#include <vector>

class ScreenshotSaver {
public:
    ScreenshotSaver();
    void CreateScreenshotFolder(const std::wstring& userName);
    void GetScreenshot(const std::wstring& userName);
    void SaveScreenshotAsPNG(const std::wstring& filePath, HBITMAP hBitmap, int width, int height);
    void ScreenCaptureThread(const std::wstring& userName);

private:
    std::wstring GetValidFileName(const std::wstring& userName);
};

#endif // SCREENSHOT_SAVER_H
