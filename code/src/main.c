#include "main.h"
#include "map.h"

int clientWidth;
int clientHeight;

vec2_t playerPos;
float playerA;

short keys[WM_KEYLAST];

uint32_t* buffer;

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

vec2_t drawClipTL;
vec2_t drawClipBR;

void setDrawClip(int x1, int y1, int x2, int y2) {
    drawClipTL.x = x1;
    drawClipTL.y = y1;

    drawClipBR.x = x2;
    drawClipBR.y = y2;
}

void setPixel(int x, int y, uint32_t color) {
    if(x < drawClipTL.x || x > drawClipBR.x || y < drawClipTL.y || y > drawClipBR.y) {
        return;
    }

    buffer[x + y * clientWidth] = color;
}

void clearBuffer(uint32_t color) {
    for(int x = 0; x < clientWidth; x++) {
        for(int y = 0; y < clientHeight; y++) {
            setPixel(x, y, color);
        }
    }
}

void drawRect(int x1, int y1, int x2, int y2, uint32_t color) {
    // Don't forget to subtract 1 pixel from final coordinate on either axis...
    for(int x = x1; x < x2; x++) {
        setPixel(x, y1, color);
        setPixel(x, y2 - 1, color);
    }
    for(int y = y1; y < y2; y++) {
        setPixel(x1, y, color);
        setPixel(x2 - 1, y, color);
    }
}

void fillRect(int x1, int y1, int x2, int y2, uint32_t color) {
    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            setPixel(x, y, color);
        }
    }
}

void drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
    int x = 0;
    int y = 0;

    int xinc1 = 0;
    int xinc2 = 0;
    int yinc1 = 0;
    int yinc2 = 0;

    float deltax = abs(x2 - x1); // The difference between the x's
    float deltay = abs(y2 - y1); // The difference between the y's
    x = x1; // Start x off at the first pixel
    y = y1; // Start y off at the first pixel

    if (x2 >= x1) // The x-values are increasing
    {
        xinc1 = 1;
        xinc2 = 1;
    }
    else // The x-values are decreasing
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) // The y-values are increasing
    {
        yinc1 = 1;
        yinc2 = 1;
    }
    else // The y-values are decreasing
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    
    float den = deltax;
    float num = deltax / 2;
    float numadd = deltay;
    float numpixels = deltax; // There are more x-values than y-values

    if (deltax >= deltay) // There is at least one x-value for every y-value
    {
        xinc1 = 0; // Don't change the x when numerator >= denominator
        yinc2 = 0; // Don't change the y for every iteration
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax; // There are more x-values than y-values
    }
    else // There is at least one y-value for every x-value
    {
        xinc2 = 0; // Don't change the x for every iteration
        yinc1 = 0; // Don't change the y when numerator >= denominator
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay; // There are more y-values than x-values
    }

    for (int curpixel = 0; curpixel <= numpixels; curpixel++)
    {
        setPixel(x, y, color); // Draw the current pixel
        num += numadd; // Increase the numerator by the top of the fraction
        if (num >= den) // Check if numerator >= denominator
        {
            num -= den; // Calculate the new numerator value
            x += xinc1; // Change the x as appropriate
            y += yinc1; // Change the y as appropriate
        }
    
        x += xinc2; // Change the x as appropriate
        y += yinc2; // Change the y as appropriate
    } 
}

float toDegrees(float radians) {
    return radians * (180.0 / M_PI);
}

float toRadians(float degrees) {
    return degrees * (M_PI / 180.0); 
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
    clientWidth = rect.right - rect.left;
    clientHeight = rect.bottom - rect.top;

    int xPos = (GetSystemMetrics(SM_CXSCREEN) - rect.right)/2;
    int yPos = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom)/2;

    SetWindowPos(hwnd, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    int bufferSize = clientWidth * clientHeight * sizeof(uint32_t);
    buffer = malloc(bufferSize);
    clearBuffer(0x000000);

    HDC hdc = GetDC(hwnd);

    BITMAPINFOHEADER bitmapInfoHeader = {0};
    bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfoHeader.biWidth = clientWidth;
    bitmapInfoHeader.biHeight = -clientHeight;
    bitmapInfoHeader.biPlanes = 1;
    bitmapInfoHeader.biBitCount = 32;
    bitmapInfoHeader.biCompression = BI_RGB;
    bitmapInfoHeader.biSizeImage = 0;
    bitmapInfoHeader.biXPelsPerMeter = 0;
    bitmapInfoHeader.biYPelsPerMeter = 0;
    bitmapInfoHeader.biClrUsed = 0;
    bitmapInfoHeader.biClrImportant = 0;

    BITMAPINFO bitmapInfo = {0};
    bitmapInfo.bmiHeader = bitmapInfoHeader;

    int c = 0;

    playerPos.x = 0;
    playerPos.y = 0;
    playerA = 0;

    struct timeb tmb;
    ftime(&tmb);
    long tickCount = tmb.time * 1000 + tmb.millitm;

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
            playerA -= 90 * dt;
        }

        if(keys['E']) {
            playerA += 90 * dt;
        }

        if(playerA < -360) {
            playerA += 360;
        }

        if(playerA > 360) {
            playerA -= 360;
        }
        
        if(keys['W']) {
            playerPos.x += sin(toRadians(playerA)) * dt * 5;
            playerPos.y += cos(toRadians(playerA)) * dt * 5;
        }
        
        if(keys['A']) {
            playerPos.x -= cos(toRadians(playerA)) * dt * 5;
            playerPos.y += sin(toRadians(playerA)) * dt * 5;
        }
        
        if(keys['S']) {
            playerPos.x -= sin(toRadians(playerA)) * dt * 5;
            playerPos.y -= cos(toRadians(playerA)) * dt * 5;
        }

        if(keys['D']) {
            playerPos.x += cos(toRadians(playerA)) * dt * 5;
            playerPos.y -= sin(toRadians(playerA)) * dt * 5;
        }

        setDrawClip(0, 0, clientWidth, clientHeight);
        clearBuffer(0x000000);

        float renderWindowSize = clientWidth / 3;

        vec2_t playerScreen = { playerPos.x, playerPos.y };
        vec2_t playerLook = { sin(toRadians(playerA)) * 1, cos(toRadians(playerA)) * 1 };
        
        // ---- Top view static ----
        {   
            vec2_t pScreen = { playerScreen.x, playerScreen.y };
            vec2_t pLScreen = { playerLook.x, playerLook.y };

            // Flip y axis
            pScreen.y *= -1;
            pLScreen.y *= -1;

            // Scale to screen coordinates
            pScreen.x = pScreen.x / g_worldWidth * renderWindowSize;
            pScreen.y = pScreen.y / g_worldHeight * renderWindowSize;

            pLScreen.x = pLScreen.x / g_worldWidth * renderWindowSize;
            pLScreen.y = pLScreen.y / g_worldHeight * renderWindowSize;

            // Move origin to center of screen
            pScreen.x += renderWindowSize / 2;
            pScreen.y += renderWindowSize / 2;

            setDrawClip(0, 0, renderWindowSize, renderWindowSize);
            drawRect(0, 0, renderWindowSize, renderWindowSize, 0xff0000);
            fillRect(pScreen.x - 1, pScreen.y - 1, pScreen.x + 1, pScreen.y + 1, 0xffffff);
            drawLine(pScreen.x, pScreen.y,  pScreen.x + pLScreen.x, pScreen.y + pLScreen.y, 0xffffff);
        }
        
        // Translate everything to player specific coordinate system
        playerScreen.x -= playerPos.x;
        playerScreen.y -= playerPos.y;

        // Rotate everything else around the player (opposite of player rotation)
        float pasin = sin(toRadians(playerA));
        float pacos = cos(toRadians(playerA));

        float temp = playerLook.x;
        playerLook.x = playerLook.x * pacos - playerLook.y * pasin;
        playerLook.y = temp * pasin + playerLook.y * pacos;

        // ---- Top view dynamic ----
        {
            vec2_t pScreen = { playerScreen.x, playerScreen.y };
            vec2_t pLScreen = { playerLook.x, playerLook.y };

            // Flip y axis
            pScreen.y *= -1;
            pLScreen.y *= -1;
                
            // Scale to screen coordinates
            pScreen.x = pScreen.x / g_worldWidth * renderWindowSize;
            pScreen.y = pScreen.y / g_worldHeight * renderWindowSize;

            pLScreen.x = pLScreen.x / g_worldWidth * renderWindowSize;
            pLScreen.y = pLScreen.y / g_worldHeight * renderWindowSize;
        
            // Move origin to center of screen
            pScreen.x += renderWindowSize + renderWindowSize / 2;
            pScreen.y += renderWindowSize / 2;

            setDrawClip(renderWindowSize, 0, renderWindowSize * 2, renderWindowSize);
            drawRect(renderWindowSize, 0, renderWindowSize * 2, renderWindowSize, 0x00ff00);
            fillRect(pScreen.x - 1, pScreen.y - 1, pScreen.x + 1, pScreen.y + 1, 0xffffff);
            drawLine(pScreen.x, pScreen.y,  pScreen.x + pLScreen.x, pScreen.y + pLScreen.y, 0xffffff);
        }

        // ---- 3D view ----
        {
            setDrawClip(renderWindowSize * 2, 0, renderWindowSize * 3, renderWindowSize);
            drawRect(renderWindowSize * 2, 0, renderWindowSize * 3, renderWindowSize, 0x0000ff);
        }

        for(int s = 0; s < g_sectorCount; s++) {
            sector_t* sector = g_sectors[s];
            for(int w = 0; w < sector->wallCount; w++) {
                wall_t* wall = g_walls[sector->walls[w]];
                vec2_t wallStartScreen = { g_corners[wall->startCorner]->x, g_corners[wall->startCorner]->y };
                vec2_t wallEndScreen = { g_corners[wall->endCorner]->x, g_corners[wall->endCorner]->y };

                // ---- Top view static ----
                {   
                    vec2_t wSScreen = { wallStartScreen.x, wallStartScreen.y };
                    vec2_t wEScreen = { wallEndScreen.x, wallEndScreen.y };

                    // Flip y axis
                    wSScreen.y *= -1;
                    wEScreen.y *= -1;
                    
                    // Scale to screen coordinates
                    wSScreen.x = wSScreen.x / g_worldWidth * renderWindowSize;
                    wSScreen.y = wSScreen.y / g_worldHeight * renderWindowSize ;
                    wEScreen.x = wEScreen.x / g_worldWidth * renderWindowSize;
                    wEScreen.y = wEScreen.y / g_worldHeight * renderWindowSize;
                    
                    // Move origin to center of screen
                    wSScreen.x += renderWindowSize / 2;
                    wSScreen.y += renderWindowSize / 2;
                    wEScreen.x += renderWindowSize / 2;
                    wEScreen.y += renderWindowSize / 2;

                    setDrawClip(0, 0, renderWindowSize, renderWindowSize);
                    drawLine(wSScreen.x, wSScreen.y,  wEScreen.x, wEScreen.y, 0xff00ff);
                }

                // Translate everything to player specific coordinate system

                wallStartScreen.x -= playerPos.x;
                wallStartScreen.y -= playerPos.y;

                wallEndScreen.x -= playerPos.x;
                wallEndScreen.y -= playerPos.y;

                // Rotate everything else around the player (opposite of player rotation)
                temp = wallStartScreen.x; 
                wallStartScreen.x = wallStartScreen.x * pacos - wallStartScreen.y * pasin;
                wallStartScreen.y = temp * pasin + wallStartScreen.y * pacos;

                temp = wallEndScreen.x;
                wallEndScreen.x = wallEndScreen.x * pacos - wallEndScreen.y * pasin;
                wallEndScreen.y = temp * pasin + wallEndScreen.y * pacos;

                // ---- Top view dynamic ----
                {
                    vec2_t wSScreen = { wallStartScreen.x, wallStartScreen.y };
                    vec2_t wEScreen = { wallEndScreen.x, wallEndScreen.y };

                    // Flip y axis
                    wSScreen.y *= -1;
                    wEScreen.y *= -1;
                    
                    // Scale to screen coordinates
                    wSScreen.x = wSScreen.x / g_worldWidth * renderWindowSize;
                    wSScreen.y = wSScreen.y / g_worldHeight * renderWindowSize ;
                    wEScreen.x = wEScreen.x / g_worldWidth * renderWindowSize;
                    wEScreen.y = wEScreen.y / g_worldHeight * renderWindowSize;
                    
                    // Move origin to center of screen
                    wSScreen.x += renderWindowSize + renderWindowSize / 2;
                    wSScreen.y += renderWindowSize / 2;
                    wEScreen.x += renderWindowSize + renderWindowSize / 2;
                    wEScreen.y += renderWindowSize / 2;

                    setDrawClip(renderWindowSize, 0, renderWindowSize * 2, renderWindowSize);
                    drawLine(wSScreen.x, wSScreen.y,  wEScreen.x, wEScreen.y, 0xff00ff);
                }

                // ---- 3D view ----
                {
                }
            }
        }

        StretchDIBits(hdc, 0, 0, clientWidth, clientHeight, 0, 0, clientWidth, clientHeight, buffer, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }

}