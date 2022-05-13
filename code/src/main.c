#include "main.h"

short keys[WM_KEYLAST];

uint32_t* buffer;

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
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
            return DefWindowProc(window, message, w_param, l_param);
        }
    }
    
    return 0;
}

vec2_t drawClipTL;
vec2_t drawClipBR;

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

void setDrawClip(int x1, int x2, int y1, int y2) {
    drawClipTL.x = x1;
    drawClipTL.y = y1;

    drawClipBR.x = x1;
    drawClipBR.y = y2;
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
                                 900,
                                 500,
                                 0,
                                 0,
                                 instance,
                                 0);
    
    if(!window) {
        MessageBox(0, L"CreateWindowEx failed", 0, 0);
        return GetLastError();
    }
    
    RECT rect;
    GetClientRect(window, &rect);
    clientWidth = rect.right - rect.left;
    clientHeight = rect.bottom - rect.top;

    int bufferSize = clientWidth * clientHeight * sizeof(uint32_t);
    buffer = malloc(bufferSize);
    clearBuffer(0x000000);

    HDC hdc = GetDC(window);

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

    int worldWidth = 20;
    int worldHeight = 20;

    wall_t wall1 = { { -8, 8 } , { 2, 8 } };
    wall_t wall2 = { { 2, 8 } , { 8, 2 } };
    wall_t wall3 = { { 8, 2 } , { 8, -8 } };
    wall_t wall4 = { { 8, -8 } , { -2, -8 } };
    wall_t wall5 = { { -2, -8 } , { -8, -2 } };
    wall_t wall6 = { { -8, -2 } , { -8, 8 } };

    sector_t sector;
    sector.wallCount = 6;
    sector.walls = malloc(sizeof(wall_t) * sector.wallCount);
    sector.walls[0] = wall1;
    sector.walls[1] = wall2;
    sector.walls[2] = wall3;
    sector.walls[3] = wall4;
    sector.walls[4] = wall5;
    sector.walls[5] = wall6;
    sector.floorHeight = 0;
    sector.ceilHeight = 4;

    playerPos.x = 0;
    playerPos.y = 0;
    playerA = 0;

    struct timeb tmb;
    ftime(&tmb);
    long tickCount = tmb.time * 1000 + tmb.millitm;


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

        drawClipTL.x = 0;
        drawClipTL.y = 0;
        drawClipBR.x = clientWidth;
        drawClipBR.y = clientHeight;

        clearBuffer(0x000000);

        float renderWindowSize = clientWidth / 3;

        for(int w = 0; w < sector.wallCount; w++) {
            wall_t wall = sector.walls[w];

            // ---- Top view static ----
            {   
                // All coordinates untransformed... 
                vec2_t playerScreen = { playerPos.x, playerPos.y };
                vec2_t wallStartScreen = { wall.start.x, wall.start.y };
                vec2_t wallEndScreen = { wall.end.x, wall.end.y };
                vec2_t playerLook = { sin(toRadians(playerA)) * 1, cos(toRadians(playerA)) * 1 };

                // No transformation before rendering needed!

                // Render screen (transform to screen coords)

                // Flip y axis
                playerScreen.y *= -1;
                wallStartScreen.y *= -1;
                wallEndScreen.y *= -1;
                playerLook.y *= -1;
                
                // Scale to screen coordinates
                playerScreen.x = playerScreen.x / worldWidth * renderWindowSize;
                playerScreen.y = playerScreen.y / worldHeight * renderWindowSize;

                wallStartScreen.x = wallStartScreen.x / worldWidth * renderWindowSize;
                wallStartScreen.y = wallStartScreen.y / worldHeight * renderWindowSize ;
                wallEndScreen.x = wallEndScreen.x / worldWidth * renderWindowSize;
                wallEndScreen.y = wallEndScreen.y / worldHeight * renderWindowSize;

                playerLook.x = playerLook.x / worldWidth * renderWindowSize;
                playerLook.y = playerLook.y / worldHeight * renderWindowSize;
                
                // Move origin to center of screen
                playerScreen.x += renderWindowSize / 2;
                playerScreen.y += renderWindowSize / 2;
                wallStartScreen.x += renderWindowSize / 2;
                wallStartScreen.y += renderWindowSize / 2;
                wallEndScreen.x += renderWindowSize / 2;
                wallEndScreen.y += renderWindowSize / 2;

                drawClipTL.x = 0;
                drawClipTL.y = 0;
                drawClipBR.x = renderWindowSize;
                drawClipBR.y = renderWindowSize;
                drawRect(0, 0, renderWindowSize, renderWindowSize, 0xff000);
                fillRect(playerScreen.x - 1, playerScreen.y - 1, playerScreen.x + 1, playerScreen.y + 1, 0xffffff);
                drawLine(playerScreen.x, playerScreen.y,  playerScreen.x + playerLook.x, playerScreen.y + playerLook.y, 0xffffff);
                drawLine(wallStartScreen.x, wallStartScreen.y,  wallEndScreen.x, wallEndScreen.y, 0xff00ff);
            }

            // ---- Top view dynamic ----
            {
                // All coordinates untransformed
                vec2_t playerScreen = { playerPos.x, playerPos.y };
                vec2_t wallStartScreen = { wall.start.x, wall.start.y };
                vec2_t wallEndScreen = { wall.end.x, wall.end.y };
                vec2_t playerLook = { sin(toRadians(playerA)) * 1, cos(toRadians(playerA)) * 1 };

                // Translate everything to player specific coordinate system
                playerScreen.x -= playerPos.x;
                playerScreen.y -= playerPos.y;

                wallStartScreen.x -= playerPos.x;
                wallStartScreen.y -= playerPos.y;

                wallEndScreen.x -= playerPos.x;
                wallEndScreen.y -= playerPos.y;

                // Rotate everything else around the player (opposite of player rotation)
                float pasin = sin(toRadians(playerA));
                float pacos = cos(toRadians(playerA));

                float temp = wallStartScreen.x; 
                wallStartScreen.x = wallStartScreen.x * pacos - wallStartScreen.y * pasin;
                wallStartScreen.y = temp * pasin + wallStartScreen.y * pacos;

                temp = wallEndScreen.x;
                wallEndScreen.x = wallEndScreen.x * pacos - wallEndScreen.y * pasin;
                wallEndScreen.y = temp * pasin + wallEndScreen.y * pacos;

                temp = playerLook.x;
                playerLook.x = playerLook.x * pacos - playerLook.y * pasin;
                playerLook.y = temp * pasin + playerLook.y * pacos;


                // Render screen (transform to screen coords)

                // Flip y axis
                playerScreen.y *= -1;
                wallStartScreen.y *= -1;
                wallEndScreen.y *= -1;
                playerLook.y *= -1;
                
                // Scale to screen coordinates
                playerScreen.x = playerScreen.x / worldWidth * renderWindowSize;
                playerScreen.y = playerScreen.y / worldHeight * renderWindowSize;

                wallStartScreen.x = wallStartScreen.x / worldWidth * renderWindowSize;
                wallStartScreen.y = wallStartScreen.y / worldHeight * renderWindowSize ;
                wallEndScreen.x = wallEndScreen.x / worldWidth * renderWindowSize;
                wallEndScreen.y = wallEndScreen.y / worldHeight * renderWindowSize;

                playerLook.x = playerLook.x / worldWidth * renderWindowSize;
                playerLook.y = playerLook.y / worldHeight * renderWindowSize;
                
                // Move origin to center of screen
                playerScreen.x += renderWindowSize + renderWindowSize / 2;
                playerScreen.y += renderWindowSize / 2;
                wallStartScreen.x += renderWindowSize + renderWindowSize / 2;
                wallStartScreen.y += renderWindowSize / 2;
                wallEndScreen.x += renderWindowSize + renderWindowSize / 2;
                wallEndScreen.y += renderWindowSize / 2;

                drawClipTL.x = renderWindowSize;
                drawClipTL.y = 0;
                drawClipBR.x = renderWindowSize * 2;
                drawClipBR.y = renderWindowSize;
                drawRect(renderWindowSize, 0, renderWindowSize * 2, renderWindowSize, 0x00ff0);
                fillRect(playerScreen.x - 1, playerScreen.y - 1, playerScreen.x + 1, playerScreen.y + 1, 0xffffff);
                drawLine(playerScreen.x, playerScreen.y,  playerScreen.x + playerLook.x, playerScreen.y + playerLook.y, 0xffffff);
                drawLine(wallStartScreen.x, wallStartScreen.y,  wallEndScreen.x, wallEndScreen.y, 0xff00ff);
            }

            // ---- 3D view ----
            {
                // All coordinates untransformed
                vec2_t playerScreen = { playerPos.x, playerPos.y };
                vec2_t wallStartScreen = { wall.start.x, wall.start.y };
                vec2_t wallEndScreen = { wall.end.x, wall.end.y };

                // Translate everything to player specific coordinate system
                wallStartScreen.x -= playerPos.x;
                wallStartScreen.y -= playerPos.y;

                wallEndScreen.x -= playerPos.x;
                wallEndScreen.y -= playerPos.y;

                // Rotate everything else around the player (opposite of player rotation)
                float pasin = sin(toRadians(playerA));
                float pacos = cos(toRadians(playerA));

                float temp = wallStartScreen.x; 
                wallStartScreen.x = wallStartScreen.x * pacos - wallStartScreen.y * pasin;
                wallStartScreen.y = temp * pasin + wallStartScreen.y * pacos;

                temp = wallEndScreen.x;
                wallEndScreen.x = wallEndScreen.x * pacos - wallEndScreen.y * pasin;
                wallEndScreen.y = temp * pasin + wallEndScreen.y * pacos;

                if(wallStartScreen.y <= 0 && wallEndScreen.y <= 0) {
                    printf("wall [%d] is entirely behind player, clip!\n", w);
                }

                drawClipTL.x = renderWindowSize * 2;
                drawClipTL.y = 0;
                drawClipBR.x = renderWindowSize * 3;
                drawClipBR.y = renderWindowSize;
                drawRect(renderWindowSize * 2, 0, renderWindowSize * 3, renderWindowSize, 0x0000ff);
            }
        }

        StretchDIBits(hdc, 0, 0, clientWidth, clientHeight, 0, 0, clientWidth, clientHeight, buffer, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    }

}