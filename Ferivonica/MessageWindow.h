#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <Windows.h>
#include <string>

class MessageWindow {
public:
    static void CreateWindowAndMessageLoop();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void DrawContent(HDC hdc, RECT rect);
};

#endif // MESSAGEWINDOW_H
