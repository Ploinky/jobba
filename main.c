#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <sys/timeb.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

typedef uint32_t u32;

u32* memory;
int client_width;
int client_height;

void draw_pixel(int x, int y, u32 color)
{
    memory[y * client_width + x] = color;
}

void clear_screen(u32 color)
{
    for(int i = 0; i < client_width * client_height; ++i)
    {
        memory[i] = color;
    }
}

LRESULT CALLBACK 
window_proc(HWND window, 
            UINT message, 
            WPARAM w_param, 
            LPARAM l_param)
{
    switch(message)
    {
        case WM_KEYDOWN:
        {
            switch(w_param)
            {
                // "o" exits the program
                case 'O':
                {
                    DestroyWindow(window);
                };
            }
        } break;
        
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

int WINAPI 
wWinMain(HINSTANCE instance, 
         HINSTANCE prev_instance, 
         PWSTR cmd_line, 
         int cmd_show)
{
    AllocConsole();

    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    // window creation
    
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
    
    if(!window)
    {
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
    
    int c = 0;

    // clear screen with gray color
    clear_screen(0x333333);

    long tickCount = clock();
    
    struct timeb tmb;
    ftime(&tmb);
    tickCount = tmb.time * 1000 + tmb.millitm;

    while(running)
    {
        ftime(&tmb);
        long now = tmb.time * 1000 + tmb.millitm;

        printf("%ld\n", (long) (1000 / (now - tickCount + 1)));

        tickCount = now;

        MSG msg;
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT) running = 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for(int x = 0; x < client_width; x++) {
            for(int y = 0; y < client_height; y++) {
                draw_pixel(x, y, 0xffffff);
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
    
    return 0;
}