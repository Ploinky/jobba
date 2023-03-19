#include "r_main.h"
#include "r_nmap.h"
rect_t g_renderTarget;

int g_windowWidth;
int g_windowHeight;

int g_clientWidth;
int g_clientHeight;

vec2_t drawClipTL;
vec2_t drawClipBR;

uint32_t* buffer;

float g_fovH = 70;

HWND hwnd;
HDC hdc;
BITMAPINFOHEADER bitmapInfoHeader;
BITMAPINFO bitmapInfo;

void R_Initialize(HWND targetHandle) {
    hwnd = targetHandle;

    int bufferSize = g_clientWidth * g_clientHeight * static_cast<int>(sizeof(uint32_t));
    buffer = (uint32_t*) malloc(static_cast<size_t>(bufferSize));
    // Clear entire screen to black
    R_SetDrawClip(rect_t{ { 0, 0 }, { static_cast<double>(g_clientWidth), static_cast<double>(g_clientHeight) } });

    hdc = GetDC(hwnd);

    bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfoHeader.biWidth = g_clientWidth;
    bitmapInfoHeader.biHeight = -g_clientHeight;
    bitmapInfoHeader.biPlanes = 1;
    bitmapInfoHeader.biBitCount = 32;
    bitmapInfoHeader.biCompression = BI_RGB;
    bitmapInfoHeader.biSizeImage = 0;
    bitmapInfoHeader.biXPelsPerMeter = 0;
    bitmapInfoHeader.biYPelsPerMeter = 0;
    bitmapInfoHeader.biClrUsed = 0;
    bitmapInfoHeader.biClrImportant = 0;

    bitmapInfo.bmiHeader = bitmapInfoHeader;
}

void R_ClearScreen(uint32_t color) {
        clearBuffer(color);
}

void R_SetDrawClip(rect_t clip) {
    setDrawClip(static_cast<int>(clip.tl.x), static_cast<int>(clip.tl.y), static_cast<int>(clip.br.x), static_cast<int>(clip.br.y));
}

void renderMap() {
    renderMapNew();
}

void setDrawClip(int x1, int y1, int x2, int y2) {
    drawClipTL.x = x1;
    drawClipTL.y = y1;

    drawClipBR.x = x2;
    drawClipBR.y = y2;
}

void setPixel(int x, int y, uint32_t color) {
    x += static_cast<int>(drawClipTL.x);
    y += static_cast<int>(drawClipTL.y);
    
    if(x < drawClipTL.x || x >= drawClipBR.x || y < drawClipTL.y || y >= drawClipBR.y) {
        // MessageBoxA(0, "Trying to draw outside window bounds!", "Rejected pixel", 0);
        return;
    }

    buffer[x + y * g_clientWidth] = color;
}

void clearBuffer(uint32_t color) {
    for(int x = 0; x < g_clientWidth; x++) {
        for(int y = 0; y < g_clientHeight; y++) {
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

    double deltax = abs(x2 - x1); // The difference between the x's
    double deltay = abs(y2 - y1); // The difference between the y's
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

    
    double den = deltax;
    double num = deltax / 2;
    double numadd = deltay;
    double numpixels = deltax; // There are more x-values than y-values

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

void R_DrawFps() {
    RECT rect;
    rect.left = 10;
    rect.right = 100;
    rect.top = 10;
    rect.bottom = 30;
    SetTextColor(hdc, 0x00FFFFFF);
    SetBkMode(hdc, TRANSPARENT);
    DrawText(hdc, L"fps: 144", -1, &rect, DT_NOCLIP);
}

void R_SwapBuffer() {
    StretchDIBits(hdc, 0, 0, g_windowWidth, g_windowHeight, 0, 0, g_clientWidth, g_clientHeight, buffer, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
    R_DrawFps();
}