#include "window.hpp"
#include <iostream>
#include <string>
#include "direct3d.hpp"
#include "model3d.hpp"
#include "logger.hpp"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCCREATE:
        {
            SetWindowLongPtr(hwnd, 0, lParam);
            break;
        }
        case WM_QUIT:
        case WM_DESTROY:
        {

            P3D::Logger::Msg("Windows window has been destroyed.");
            P3D::Window *window = (P3D::Window *)GetWindowLongPtr(hwnd, 0);
            window->SetShouldClose();
            break;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

namespace P3D {
    Window::Window() {
        Logger::Msg("Creating Windows window!");

        LPCWSTR className = L"Ploinky's 3D Engine";

        shouldClose = false;

        // ------ Create window -----
        WNDCLASSEXW wc;
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_HAND);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className;
        wc.hIconSm = wc.hIcon;

        int posX, posY;
        int width = 1024;
        int height = 800;

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        screenWidth = width;
        screenHeight = height;

        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

        RegisterClassExW(&wc);

        DWORD dwStyle = WS_OVERLAPPEDWINDOW;

        RECT wr = {0, 0, width, height};       // set the size, but not the position
        AdjustWindowRect(&wr, dwStyle, FALSE); // adjust the window's size

        windowHandle = CreateWindowExW(WS_EX_APPWINDOW,
                                       wc.lpszClassName,
                                       wc.lpszClassName,
                                       dwStyle,
                                       posX,
                                       posY,
                                       wr.right - wr.left,
                                       wr.bottom - wr.top,
                                       NULL,
                                       NULL,
                                       GetModuleHandle(NULL),
                                       NULL);

        if (windowHandle == 0) {
            Logger::Err("WindowHandle is NULL.");
            Logger::Err(std::to_string(GetLastError()));
            return;
        }

        ShowWindow(windowHandle, SW_SHOW);
    }

    Window::~Window() {
    }

    void Window::Show() {
        Logger::Msg("Windows window showing!");
    }

    // Handle win32 window events
    void Window::HandleEvents() {
        MSG msg = {};

        // Use PeekMessage, GetMessage blocks!
        if (PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    bool Window::ShouldClose() {
        return shouldClose;
    }

    void Window::SetShouldClose() {
        shouldClose = true;
    }
    
    HWND Window::GetWindowHandle() {
        return windowHandle;
    }
}