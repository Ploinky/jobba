#define WIN32_LEAN_AND_MEAN
#define UNICODE

#include <Windows.h>
#include "renderer.hpp"
#include <string>
#include "version.h"

static Renderer renderer{};
LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_KEYDOWN:
    {
        //g_keys[wParam] = 1;
        break;
    }
    case WM_KEYUP:
    {
        //g_keys[wParam] = 0;
        break;
    }
    case WM_SIZE:
    {
        renderer.Resize(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_ERASEBKGND:
    {
        return 1;
    }
    case WM_CREATE:
    {
        renderer.Initialize(hwnd, 320, 200);
        break;
    }
    default:
    {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    }

    return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int cmd_show) {
    WNDCLASS windowClass = { 0 };

    const wchar_t windowClassName[] = L"JobbaWindow";

    windowClass.lpfnWndProc = window_proc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = windowClassName;
    windowClass.hCursor = LoadCursor(0, IDC_CROSS);

    if (!RegisterClass(&windowClass))
    {
        MessageBox(0, L"RegisterClass failed", 0, 0);
        return static_cast<int>(GetLastError());
    }

#ifdef _DEBUG
    std::wstring windowTitle = std::wstring(L"Jobba development build - v").append(std::wstring(JOBBA_VERSION));
#else
    const wchar_t* windowTitle = L"Jobba";
#endif

    HWND hwnd = CreateWindowEx(0,
        windowClassName,
        windowTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1024,
        768,
        0,
        0,
        instance,
        0);

    if (!hwnd) {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return static_cast<int>(GetLastError());
    }

    if (ShowWindow(hwnd, cmd_show)) {
        MessageBox(0, L"ShowWindow failed", 0, 0);
        return static_cast<int>(GetLastError());
    }

    bool is_running = true;

    while (is_running) {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) {
                is_running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        renderer.RenderMap();
        renderer.RenderMainMenu();

        renderer.FlipBackBuffer();
    }
}