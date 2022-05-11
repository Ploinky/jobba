#include "main.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

void draw_pixel(int x, int y, u32 color) {
    memory[(y * client_width + x)] = color;
}

void clear_screen(u32 color) {
    for(int i = 0; i < client_width * client_height; ++i)
    {
        memory[i] = color;
    }
}

short keys[WM_KEYLAST];

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    switch(message)
    {
        case WM_KEYDOWN:
        {
            keys[w_param] = 1;
        } break;

        case WM_KEYUP:
        {
            keys[w_param] = 0;
            break;
        }
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        } break;

        default:
        {
            return DefWindowProc(window, 
                                 message, 
                                 w_param, 
                                 l_param);
        }
    }
    
    return 0;
}

float cross(vec2_t a, vec2_t b) {
    return a.x * b.y - a.y * b.x;
}

vec2_t subtract(vec2_t a, vec2_t b) {
    vec2_t ret = {a.x - b.x, a.y - b.y};

    return ret;
}

float length(vec2_t vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}


float toDegrees(float radians) {
    return radians * (180.0 / M_PI);
}

float toRadians(float degrees) {
    return degrees * (M_PI / 180.0); 
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show) {
    AllocConsole();

    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    
    WNDCLASS window_class = {0};
    
    const wchar_t class_name[] = L"MyWindowClass";
    
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    window_class.hCursor = LoadCursor(0, IDC_CROSS);
    
    if(!RegisterClass(&window_class))
    {
        MessageBox(0, L"RegisterClass failed", 0, 0);
        return GetLastError();
    }
    
    HWND window = CreateWindowEx(0,
                                 class_name,
                                 L"Window",
                                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 1920,
                                 1080,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    if(!window) {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return GetLastError();
    }
    
    // allocate memory
    
    RECT rect;
    GetClientRect(window, &rect);
    client_width = rect.right - rect.left;
    client_height = rect.bottom - rect.top;
    
    memory = VirtualAlloc(0,
                          client_width * client_height * 4,
                          MEM_RESERVE|MEM_COMMIT,
                          PAGE_READWRITE
                          );

    float fovH = 90;
    float fovV = fovH * (((float) client_height) / ((float) client_width));
        
    // create BITMAPINFO struct for StretchDIBits
    
    BITMAPINFO bitmap_info;
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = GetDC(window);
    
    // loop
    
    int running = 1;

    // clear screen with gray color
    clear_screen(0x333333);

    long tickCount = clock();
    
    struct timeb tmb;
    ftime(&tmb);
    tickCount = tmb.time * 1000 + tmb.millitm;

    playerHeight = 2;

    int wallCount = 5;
    wall_t* walls = malloc(sizeof(wall_t) * wallCount);

    wall_t wall1 = {0, 0, 30, 0, 0, 4};
    wall_t wall2 = {30, 0, 30, 30, 0, 4};
    wall_t wall3 = {30, 30, 0, 30, 0, 4};
    wall_t wall4 = {0, 30, 0, 0, 0, 4};
    wall_t wall5 = {5, 5, 20, 1, 0, 2};

    walls[0] = wall1;
    walls[1] = wall2;
    walls[2] = wall3;
    walls[3] = wall4;
    walls[4] = wall5;

    u32* colors = malloc(sizeof(u32) * wallCount);
    colors[0] = 0xff0000;
    colors[1] = 0x00ff00;
    colors[2] = 0x0000ff;
    colors[3] = 0x000000;
    colors[4] = 0xff00ff;

    playerX = 15;
    playerY = 15;

    playerA = 0;

    for(int i = 0; i < WM_KEYLAST; i++) {
        keys[i] = 0;
    }

    while(running)
    {
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
            running = 0;
        }
        
        if(keys['E']) {
            playerA += 90.0f * dt;
        }

        if(keys['Q']) {
            playerA -= 90.0f * dt;
        }

        float newX = cos(toRadians(playerA)) * (float)(keys['W'] - keys['S']) - sin(toRadians(playerA)) * (float) (keys['D'] - keys['A']);
        float newY = sin(toRadians(playerA)) * (float)(keys['W'] - keys['S']) + cos(toRadians(playerA)) * (float)(keys['D'] - keys['A']);
        
        playerX += newX * dt * 5.0f;
        playerY += newY * dt * 5.0f;
        

        if(playerA > 360.0f) {
            playerA -= 360;
        } else if(playerA < -360.0f) {
            playerA += 360;
        }

        for(int w = 0; w < wallCount; w++) {

            vec2_t p = { playerX, playerY};
            vec2_t q = { walls[w].startX, walls[w].startY };
            vec2_t s = { walls[w].endX - walls[w].startX, walls[w].endY - walls[w].startY };

            for(int x = 0; x < client_width; x++) {

                float rayAngle = (playerA - fovV / 2.0f) + (x / ((float) client_width)) * fovV;

                vec2_t r = { cos(toRadians(rayAngle)), sin(toRadians(rayAngle)) };
                
                if(cross(r, s) == 0) {
                    continue;
                }

                float u = cross(subtract(q, p), r) / cross(r, s);

                if(u < 0 || u > 1) {
                    continue;
                }

                float t = cross(subtract(q, p), s) / cross(r, s);

                if(t < 0 ) {
                    continue;
                }

                vec2_t intersect = { p.x + r.x * t, p.y + r.y * t };
                vec2_t dist = { intersect.x - playerX, intersect.y - playerY };

                float a = walls[w].height;
                float c = length(dist);
                float b = sqrt(a * a + c * c);

                float oA = walls[w].posY;
                float oB = sqrt(oA * oA + c * c);

                float angle = acos((b*b + c*c - a*a) / (2.0f * b * c));
                float angleOffset = acos((oB*oB + c*c - oA*oA) / (2.0f * oB * c));
                float height = ((float) client_height) * (toDegrees(angle) / fovV);
                float offset = ((float) client_height) * (toDegrees(angleOffset) / fovV);

                if(oA < 0) {
                    offset *= -1;
                }

                int startY = max(0.0f, (((float) client_height) - height) / 2 + offset);
                int endY = min(client_height - 1, startY + height);

                for(int y = 0; y < startY; y++) {
                    draw_pixel(x, y, 0x0f0f0f);
                }
                for(int y = startY; y <= endY; y++) {
                    draw_pixel(x, y, colors[w]);
                }
                for(int y = endY + 1; y < client_height; y++) {
                    draw_pixel(x, y, 0xf0f0f0);
                }
            }
        }
        
        StretchDIBits(hdc,
                      0,
                      0,
                      client_width,
                      client_height,
                      0,
                      0,
                      client_width,
                      client_height,
                      memory,
                      &bitmap_info,
                      DIB_RGB_COLORS,
                      SRCCOPY
                      );
   }

   VirtualFree(memory, client_width * client_height * 4, MEM_RELEASE);
   free(walls);
   free(colors);
    
    return 0;
}