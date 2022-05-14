#include "main.h"
#include "map.h"
#include "r_main.h"

vec2_t g_playerPos;
float g_playerA;

short keys[WM_KEYLAST];

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    switch(message)
    {
        case WM_KEYDOWN:
        {
            keys[w_param] = 1;
            break;
        }
        case WM_KEYUP:
        {
            keys[w_param] = 0;
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefWindowProc(hwnd, message, w_param, l_param);
        }
    }
    
    return 0;
}

float toDegrees(float radians) {
    return radians * (180.0 / M_PI);
}

float toRadians(float degrees) {
    return degrees * (M_PI / 180.0); 
}

vec2_t subtract(vec2_t a, vec2_t b) {
    vec2_t ret = { a.x - b.x, a.y - b.y};
    return ret;
}

float cross(vec2_t a, vec2_t b) {
    return a.x * b.y - a.y * b.x;
}


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show) {
    // Allocate a console for debugging
    AllocConsole();

    // Reroute standard input and output to console
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

    WNDCLASS window_class = {0};
    
    const wchar_t class_name[] = L"JobbaWindow";
    
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    window_class.hCursor = LoadCursor(0, IDC_CROSS);
    
    if(!RegisterClass(&window_class))
    {
        MessageBox(0, L"RegisterClass failed", 0, 0);
        return GetLastError();
    }
    
    HWND hwnd = CreateWindowEx(0,
                                 class_name,
                                 L"Jobba development build - v0.0.1a",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 900,
                                 500,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    if(!hwnd) {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return GetLastError();
    }
    
    RECT rect;
    GetClientRect(hwnd, &rect);
    g_clientWidth = rect.right - rect.left;
    g_clientHeight = rect.bottom - rect.top;

    int xPos = (GetSystemMetrics(SM_CXSCREEN) - rect.right)/2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom)/2;

    SetWindowPos(hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);


    int c = 0;

    g_playerPos.x = 0;
    g_playerPos.y = 0;
    g_playerA = 0;

    struct timeb tmb;
    ftime(&tmb);
    long tickCount = tmb.time * 1000 + tmb.millitm;

    R_Initialize(hwnd);
    LoadMap();

    int running = 1;
    while(running) {
        
        ftime(&tmb);
        long now = tmb.time * 1000 + tmb.millitm;

        float dt = (now - tickCount) / 1000.0f;

        tickCount = now;

        MSG msg;
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT) running = 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(keys[VK_ESCAPE]) {
            PostQuitMessage(0);
        }

        if(keys['Q']) {
            g_playerA -= 90 * dt;
        }

        if(keys['E']) {
            g_playerA += 90 * dt;
        }

        if(g_playerA < -360) {
            g_playerA += 360;
        }

        if(g_playerA > 360) {
            g_playerA -= 360;
        }
        
        if(keys['W']) {
            g_playerPos.x += sin(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y += cos(toRadians(g_playerA)) * dt * 5;
        }
        
        if(keys['A']) {
            g_playerPos.x -= cos(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y += sin(toRadians(g_playerA)) * dt * 5;
        }
        
        if(keys['S']) {
            g_playerPos.x -= sin(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y -= cos(toRadians(g_playerA)) * dt * 5;
        }

        if(keys['D']) {
            g_playerPos.x += cos(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y -= sin(toRadians(g_playerA)) * dt * 5;
        }

        // Clear entire screen to black
        R_SetDrawClip((rect_t) {0, 0, g_clientWidth, g_clientHeight});
        R_ClearScreen(0x000000);

        float renderWindowSize = g_clientWidth / 3;
        
        // ---- Top view static ----
        {   
            setDrawClip(0, 0, renderWindowSize, renderWindowSize);
            g_mapRenderMode = RENDER_MAP_STATIC;
            renderMap();
        }

        // ---- 3D view ----
        {
            setDrawClip(renderWindowSize, 0, renderWindowSize * 2, renderWindowSize);
            g_mapRenderMode = RENDER_MAP_DYNAMIC;
            renderMap();
        }
        
        R_SwapBuffer();
    }
}