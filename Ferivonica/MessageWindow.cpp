#include "MessageWindow.h"

LRESULT CALLBACK MessageWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        DrawContent(hdc, rect);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void MessageWindow::DrawContent(HDC hdc, RECT rect) {
    // Draw the greeting message
    DrawText(hdc, L"Hello, my dear programmer", -1, &rect, DT_CENTER | DT_SINGLELINE);

    // Calculate the height of the greeting text
    int textHeight = DrawText(hdc, L"Hello, my dear programmer", -1, &rect, DT_CENTER | DT_SINGLELINE);
    rect.top += textHeight + 20; // Move down below the greeting text

    // Set font size for the "Wishing you blessed Ramadan" message
    HFONT hFont = CreateFont(40, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
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
}

void MessageWindow::CreateWindowAndMessageLoop() {
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
