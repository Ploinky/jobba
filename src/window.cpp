#include "window.hpp"
#include <windowsx.h>
#include <iostream>
#include <string>
#include "direct3d.hpp"
#include "mesh.hpp"
#include "logger.hpp"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_QUIT:
        case WM_DESTROY:
        {
            P3D::Logger::Msg("Windows window has been destroyed.");
            P3D::Window *window = (P3D::Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            window->SetShouldClose();
            break;
        }
        case WM_SIZE:
        {
            P3D::Window *window = (P3D::Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

            if(window == nullptr) {
                break;
            }
            
            window->Resized(LOWORD(lParam), HIWORD(lParam));

            RECT rect;
            GetClientRect(hwnd, &rect);

            POINT ul;
            ul.x = rect.left;
            ul.y = rect.top;

            POINT lr;
            lr.x = rect.right;
            lr.y = rect.bottom;

            MapWindowPoints(hwnd, nullptr, &ul, 1);
            MapWindowPoints(hwnd, nullptr, &lr, 1);

            rect.left = ul.x;
            rect.top = ul.y;

            rect.right = lr.x;
            rect.bottom = lr.y;

            ClipCursor(&rect);
            break;
        }
        case WM_KEYDOWN:
        {
            P3D::Window *window = (P3D::Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if(window != nullptr) {
                window->KeyPressed(wParam);
            }
            break;
        }
        case WM_KEYUP:
        {
            P3D::Window *window = (P3D::Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if(window != nullptr) {
                window->KeyReleased(wParam);
            }
            break;
        }
        case WM_MOUSEMOVE:
        {
            P3D::Window *window = (P3D::Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if(window != nullptr) {
                window->MouseMoved(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            }

            break;
        }
        case WM_INPUT:
        {
            P3D::Window *window = (P3D::Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if(window == nullptr) {
                break;
            }

            RAWINPUT* rawBuffer;
            UINT cbSize;

            GetRawInputData((HRAWINPUT)lParam, 
                            RID_INPUT, 
                            NULL, 
                            &cbSize,
                            sizeof(RAWINPUTHEADER)
                            );
            
	        rawBuffer = (PRAWINPUT)malloc(cbSize);

            if(!rawBuffer) {
                return FALSE;
            }
           
            if(GetRawInputData((HRAWINPUT) lParam, RID_INPUT, rawBuffer, &cbSize, sizeof(RAWINPUTHEADER))) {
                if(rawBuffer->header.dwType == RIM_TYPEMOUSE) {
                    window->MouseMoved(rawBuffer->data.mouse.lLastX, rawBuffer->data.mouse.lLastY);
                } else if(rawBuffer->header.dwType == RIM_TYPEKEYBOARD) {
                    if(rawBuffer->data.keyboard.Flags == RI_KEY_MAKE) {
                        window->KeyPressed(rawBuffer->data.keyboard.VKey);
                    } else if(rawBuffer->data.keyboard.Flags == RI_KEY_BREAK) {
                        window->KeyReleased(rawBuffer->data.keyboard.VKey);
                    }
                }
            }
            
            free(rawBuffer);

            return TRUE;
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
        width = 1024;
        height = 800;

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        posX = (screenWidth - width) / 2;
        posY = (screenHeight - height) / 2;

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
        
        // Save pointer to P3D::Window for WndProc to access
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, LONG_PTR(this));

        if (windowHandle == 0) {
            Logger::Err("WindowHandle is NULL.");
            Logger::Err(std::to_string(GetLastError()));
            return;
        }

        RAWINPUTDEVICE Rid[2];
                
        Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
        Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
        Rid[0].dwFlags = RIDEV_NOLEGACY;    // adds mouse and also ignores legacy mouse messages
        Rid[0].hwndTarget = 0;

        Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
        Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
        Rid[1].dwFlags = RIDEV_NOLEGACY;    // adds keyboard and also ignores legacy keyboard messages
        Rid[1].hwndTarget = 0;

        if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) {
            //registration failed. Call GetLastError for the cause of the error
        }

        ShowCursor(false);
    }

    Window::~Window() {
    }

    void Window::Show() {
        Logger::Msg("Windows window showing!");
        ShowWindow(windowHandle, SW_SHOW);
    }

    MSG msg = {};
    
    // Handle win32 window events
    void Window::HandleEvents() {
        // Use PeekMessage, GetMessage blocks!
        while (PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE) > 0)
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

    void Window::Resized(int width, int height) {
        this->width = width;
        this->height = height;

        if(windowResizedHandler != nullptr) {
            windowResizedHandler();
        }
    }

    void Window::KeyPressed(long key) {
        if(keyHandler != nullptr) {
            keyHandler(key, true);
        }
    }

    void Window::KeyReleased(long key) {
        if(keyHandler != nullptr) {
            keyHandler(key, false);
        }
    }

    void Window::MouseMoved(unsigned short x, unsigned short y) {
        if(mouseHandler != nullptr) {
            mouseHandler(x, y);
        }
    }
}