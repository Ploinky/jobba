#include "main.h"
#include "r_main.h"
#include <cstdlib>

vec2_t g_playerPos;

double g_playerA;

short g_keys[WM_KEYLAST];

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message)
    {
        case WM_KEYDOWN:
        {
            g_keys[wParam] = 1;
            break;
        }
        case WM_KEYUP:
        {
            g_keys[wParam] = 0;
            break;
        }
        case WM_SIZE:
        {
            g_windowWidth = LOWORD(lParam);
            g_windowHeight = HIWORD(lParam);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }
    
    return 0;
}

double toDegrees(double radians) {
    return radians * (180.0 / M_PI);
}

double toRadians(double degrees) {
    return degrees * (M_PI / 180.0); 
}

vec2_t subtract(vec2_t a, vec2_t b) {
    vec2_t ret = { a.x - b.x, a.y - b.y};
    return ret;
}

double cross(vec2_t a, vec2_t b) {
    return a.x * b.y - a.y * b.x;
}


int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int cmd_show) {
#ifdef _DEBUG
    // Allocate a console for debugging
    AllocConsole();

    // Reroute standard input and output to console
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
#endif

    WNDCLASS windowClass = {0};
    
    const wchar_t windowClassName[] = L"JobbaWindow";
    
    windowClass.lpfnWndProc = window_proc;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = windowClassName;
    windowClass.hCursor = LoadCursor(0, IDC_CROSS);
    
    if(!RegisterClass(&windowClass))
    {
        MessageBox(0, L"RegisterClass failed", 0, 0);
        return static_cast<int>(GetLastError());
    }
    
#ifdef _DEBUG
    const wchar_t* windowTitle = L"Jobba development build - v0.0.1a";
#else
    const wchar_t* windowTitle = L"Jobba";
#endif

    HWND hwnd = CreateWindowEx(0,
                                 windowClassName,
                                 windowTitle,
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 1024,
                                 768,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    if(!hwnd) {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return static_cast<int>(GetLastError());
    }

    if (ShowWindow(hwnd, cmd_show)) {
        MessageBox(0, L"ShowWindow failed", 0, 0);
        return static_cast<int>(GetLastError());
    }
    
    g_clientWidth = 1024;
    g_clientHeight = 768;
    
    g_windowWidth = 1024;
    g_windowHeight = 768;
    
    RECT rect;
    GetClientRect(hwnd, &rect);

    int xPos = (GetSystemMetrics(SM_CXSCREEN) - rect.right)/2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom)/2;

    SetWindowPos(hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    g_playerPos.x = 0;
    g_playerPos.y = 0;
    g_playerA = 0;

    struct timeb tmb;
    ftime(&tmb);
    uint32_t tickCount = static_cast<uint32_t>(tmb.time) * 1000 + static_cast<uint32_t>(tmb.millitm);

    R_Initialize(hwnd);

    int running = 1;
    while(running) {
        ftime(&tmb);
        uint32_t now = static_cast<uint32_t>(tmb.time) * 1000 + static_cast<uint32_t>(tmb.millitm);

        double dt = (now - tickCount) / 1000.0;

        tickCount = now;

        MSG msg;
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT) running = 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(g_keys[VK_ESCAPE]) {
            PostQuitMessage(0);
        }

        if(g_keys['Q']) {
            g_playerA -= 90.0 * dt;
        }

        if(g_keys['E']) {
            g_playerA += 90 * dt;
        }

        if(g_playerA < -360) {
            g_playerA += 360;
        }

        if(g_playerA > 360) {
            g_playerA -= 360;
        }
        
        if(g_keys['W']) {
            g_playerPos.x += cos(toRadians(g_playerA)) * dt * 5.0;
            g_playerPos.y += sin(toRadians(g_playerA)) * dt * 5.0;
        }
        
        if(g_keys['D']) {
            g_playerPos.x -= sin(toRadians(g_playerA)) * dt * 5.0;
            g_playerPos.y += cos(toRadians(g_playerA)) * dt * 5.0;
        }
        
        if(g_keys['S']) {
            g_playerPos.x -= cos(toRadians(g_playerA)) * dt * 5.0;
            g_playerPos.y -= sin(toRadians(g_playerA)) * dt * 5.0;
        }

        if(g_keys['A']) {
            g_playerPos.x += sin(toRadians(g_playerA)) * dt * 5.0;
            g_playerPos.y -= cos(toRadians(g_playerA)) * dt * 5.0;
        }    
        
        if(!g_keys['P']) {
                R_ClearScreen(0x000000);
        }

        setDrawClip(0, 0, g_clientWidth, g_clientHeight);
        renderMap();
        
        R_SwapBuffer();

        printf("dt: %f, %d fps\n", dt, (int) (1/dt));
    }

    return 0;
}