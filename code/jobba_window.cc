#include "jobba_window.h"

#include <string>
#include "settings.h"
#include "version.h"

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_KEYDOWN: {
        // TODO: Input system
        if (wParam == VK_SPACE) {
            Settings::current_resolution = NextResolution(Settings::current_resolution);
            Settings::current_virtual_resolution = Settings::current_resolution;
        }

        if (wParam == VK_UP) {
            Settings::current_virtual_resolution = NextResolution(Settings::current_virtual_resolution);
        }

        if (wParam == VK_DOWN) {
            Settings::current_resolution = NextResolution(Settings::current_resolution);
        }

        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }

        if (wParam == VK_RETURN) {
            DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);
            DWORD dwRemove = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
            DWORD dwNewStyle = dwStyle & ~dwRemove;
            ::SetWindowLong(hwnd, GWL_STYLE, dwNewStyle);
            ::SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            HDC hdc = ::GetWindowDC(NULL);
            ::SetWindowPos(hwnd, NULL, 0, 0, ::GetDeviceCaps(hdc, HORZRES), ::GetDeviceCaps(hdc, VERTRES), SWP_FRAMECHANGED);
        }
        break;
    }
    case WM_KEYUP: {
        //g_keys[wParam] = 0;
        break;
    }
    case WM_SIZE: {
        // renderer.Resize(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    case WM_ERASEBKGND: {
        return 1;
    }
    case WM_CREATE: {
        // renderer.Initialize(hwnd, 320, 180);
        break;
    }
    default: {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    }

    return 0;
}

JobbaWindow::JobbaWindow(JobbaResolution initial_resolution) {
    window_resolution_ = initial_resolution;

    const wchar_t* window_class_name = L"JobbaWindow";

    WNDCLASS window_class = { 0 };
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.lpszClassName = window_class_name;
    window_class.hCursor = LoadCursor(0, IDC_CROSS);

    if (!RegisterClass(&window_class)) {
        MessageBox(0, L"RegisterClass failed", 0, 0);
    }

#ifdef _DEBUG
    std::wstring window_title = std::wstring(L"Jobba development build - v").append(std::wstring(JOBBA_VERSION));
#else
    std::wstring window_title = L"Jobba";
#endif

    DWORD window_style = WS_OVERLAPPEDWINDOW & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX;
    DWORD window_style_ex = 0;

    unsigned int width;
    unsigned int height;
    GetJobbaResolutionValue(window_resolution_, &width, &height);
    RECT rect{ 0, 0, width, height };
    AdjustWindowRectEx(&rect, window_style, false, window_style_ex);

    window_handle_ = CreateWindowEx(window_style_ex,
        window_class_name,
        window_title.c_str(),
        window_style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0,
        0,
        GetModuleHandle(NULL),
        0);
}

HWND JobbaWindow::get_window_handle() {
    return window_handle_;
}

JobbaResolution JobbaWindow::get_current_resolution() {
    return window_resolution_;
}

void JobbaWindow::Show(int cmd_show) {
    ShowWindow(window_handle_, cmd_show);
}

void JobbaWindow::SetResolution(JobbaResolution new_resolution) {
    window_resolution_ = new_resolution;

    unsigned int width;
    unsigned int height;
    GetJobbaResolutionValue(window_resolution_, &width, &height);
    DWORD window_style = WS_OVERLAPPEDWINDOW;
    DWORD window_style_ex = 0;
    RECT rect{ 0, 0, width, height };
    AdjustWindowRectEx(&rect, window_style, false, window_style_ex);
    SetWindowPos(window_handle_, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
}