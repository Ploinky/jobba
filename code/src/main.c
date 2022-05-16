#include "main.h"
#include "map.h"
#include "r_main.h"

vec2_t g_playerPos;
float g_playerA;

short g_keys[WM_KEYLAST];

LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    switch(message)
    {
        case WM_KEYDOWN:
        {
            g_keys[w_param] = 1;
            break;
        }
        case WM_KEYUP:
        {
            g_keys[w_param] = 0;
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
                                 860,
                                 480,
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

    g_playerPos.x = 0;
    g_playerPos.y = 0;
    g_playerA = 0;

    struct timeb tmb;
    ftime(&tmb);
    uint64_t tickCount = tmb.time * 1000 + tmb.millitm;

    R_Initialize(hwnd);
    LoadMap();

    g_mapRenderMode = RENDER_MAP_STATIC;

    int running = 1;
    while(running) {
        
        ftime(&tmb);
        uint64_t now = tmb.time * 1000.0 + tmb.millitm;

        double dt = (now - tickCount) / 1000.0;

        tickCount = now;
        
        RECT rect;
        rect.left = 10;
        rect.top = 10;
        rect.right = 110;
        rect.bottom = 110;

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
            g_playerA -= 90 * dt;
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
            g_playerPos.x += sin(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y += cos(toRadians(g_playerA)) * dt * 5;
        }
        
        if(g_keys['A']) {
            g_playerPos.x -= cos(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y += sin(toRadians(g_playerA)) * dt * 5;
        }
        
        if(g_keys['S']) {
            g_playerPos.x -= sin(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y -= cos(toRadians(g_playerA)) * dt * 5;
        }

        if(g_keys['D']) {
            g_playerPos.x += cos(toRadians(g_playerA)) * dt * 5;
            g_playerPos.y -= sin(toRadians(g_playerA)) * dt * 5;
        }

        // Clear entire screen to black
        R_SetDrawClip((rect_t) {0, 0, g_clientWidth, g_clientHeight});
        R_ClearScreen(0x000000);

        float renderWindowSize = g_clientWidth / 2;

        if(g_keys['V']) {
            switch(g_mapRenderMode) {
                case RENDER_MAP_STATIC:
                    g_mapRenderMode = RENDER_MAP_DYNAMIC;
                    break;
                case RENDER_MAP_DYNAMIC:
                    g_mapRenderMode = RENDER_MAP_PERSPECTIVE;
                    break;
                case RENDER_MAP_PERSPECTIVE:
                    g_mapRenderMode = RENDER_MAP_STATIC;
                    break;
            }
            g_keys['V'] = 0;
        }

        setDrawClip(0, 0, g_clientWidth, g_clientHeight);
        renderMap();
        // ---- Top view static ----
        if(0) {
            {   
                setDrawClip(0, 0, renderWindowSize, renderWindowSize);
                g_mapRenderMode = RENDER_MAP_STATIC;
                renderMap();
            }

            // ---- Top view dynamic ----
            {
                setDrawClip(renderWindowSize, 0, renderWindowSize * 2, renderWindowSize);
                g_mapRenderMode = RENDER_MAP_DYNAMIC;
                renderMap();
            }

            // ---- 3D view ----
            {
                setDrawClip(renderWindowSize * 2, 0, renderWindowSize * 3, renderWindowSize);
                g_mapRenderMode = RENDER_MAP_PERSPECTIVE;
                renderMap();
            }
        }
        
        R_SwapBuffer();
    }
}