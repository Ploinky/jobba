#include "renderer.h"
#include <exception>
#include <string>

Renderer::~Renderer() {
}
void Renderer::Initialize(HWND hwnd, unsigned int width, unsigned int height) {
    window_width_ = width;
    window_height_ = height;

    render_width_ = 320;
    render_height_ = 180;

    window_handle_ = hwnd;
    hdc_ = GetDC(window_handle_);

    InitializeBitmapinfo();
    InitializeDataBuffer();
    InitializeBackbuffer();
};

void Renderer::SetPixel(unsigned int x, unsigned int y, UINT32 color) {
    pixel_data_[x + y * render_width_] = color;
}

const char map[10][10] = {
    {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', '.', '.', '.', '.', '.', '.', '.', 'x'},
    {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'}
};

void Renderer::RenderMap(float dirX, float dirY) {
    float posX = 5.5f;
    float posY = 5.5f;
        
    float sideDistX;
    float sideDistY;

    for (int x = 0; x < render_width_; x++) {
        int mapX = 5;
        int mapY = 5;

        double cameraX = 2 * x / double(render_width_) - 1;
        float rayDirX = dirX + cameraX;
        float rayDirY = dirY + cameraX * 0.66f;

        float deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
        float deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

        double perpWallDist;

        int stepX;
        int stepY;

        int hit = 0;
        int side;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        //perform DDA
        while (hit == 0)
        {
            //jump to next map square, either in x-direction, or in y-direction
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            //Check if ray has hit a wall
            if (map[mapX][mapY] == 'x') hit = 1;
        }

        //Calculate distance projected on camera direction (Euclidean distance would give fisheye effect!)
        if (side == 0) perpWallDist = (sideDistX - deltaDistX);
        else          perpWallDist = (sideDistY - deltaDistY);

        int h = render_width_ / 2;

        //Calculate height of line to draw on screen
        int lineHeight = (int)(h / perpWallDist);

        //calculate lowest and highest pixel to fill in current stripe
        int drawStart = -lineHeight / 2 + h / 2;
        if (drawStart < 0)drawStart = 0;
        int drawEnd = lineHeight / 2 + h / 2;
        if (drawEnd >= h)drawEnd = h - 1;

        for (int y = 0; y < drawStart; y++) {
            SetPixel(x, y, 0xFF0000);
        }
        for (int y = drawStart; y < drawEnd; y++) {
            SetPixel(x, y, 0x00FF00);
        }
        for (int y = drawEnd; y < render_height_; y++) {
            SetPixel(x, y, 0x0000FF);
        }
    }
    /*

    return;

    for (int x = 0; x < render_width_; x++) {
        for (int y = 0; y < render_height_; y++) {
            UINT32 color = 0x00FF00;
            if (x > 300 || y > 160) {
                color = 0x0000FF;
            }

            if (x == 319 || y == 179) {
                color = 0xFF00FF;
            }

            SetPixel(x, y, color);
        }
    }
    */

    unsigned int start_x, end_x, start_y, end_y;
    double ratio_w = (double) window_width_ / (double) render_width_;
    double ratio_h = (double) window_height_ / (double) render_height_;
    double ratio = min(ratio_w, ratio_h);

    end_x = static_cast<unsigned int>(render_width_ * ratio);
    end_y = static_cast<unsigned int>(render_height_ * ratio);

    start_x = (window_width_ - end_x) / 2;
    start_y = (window_height_ - end_y) / 2;

    StretchDIBits(hdc_backbuffer_, start_x, start_y, end_x, end_y, 0, 0, render_width_, render_height_, pixel_data_, &bitmapinfo_, DIB_RGB_COLORS, SRCCOPY);
}

unsigned int f = 0;

void Renderer::RenderDebugInfo() {
    RECT rect{ 0, 0, 100, 100 };
    FillRect(hdc_backbuffer_, &rect, solid_brush_);
    SetBkMode(hdc_backbuffer_, TRANSPARENT);
    DrawText(hdc_backbuffer_, std::to_wstring(f++).c_str(), -1, &rect, NULL);
}

void Renderer::RenderMainMenu() {
    RECT rect{ window_width_ / 2 - 200, window_height_ / 2 - 10, window_width_ / 2 + 200, window_height_ / 2 + 10 };
    FillRect(hdc_backbuffer_, &rect, solid_brush_);
    SetBkMode(hdc_backbuffer_, TRANSPARENT);
    DrawText(hdc_backbuffer_, std::to_wstring(f++).c_str(), -1, &rect, DT_CENTER | DT_VCENTER);
}

void Renderer::FlipBackBuffer() {
    RECT rc;
    GetClientRect(window_handle_, &rc);

    BitBlt(hdc_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hdc_backbuffer_, 0, 0, SRCCOPY);
}

void Renderer::InitializeDataBuffer() {
    if (pixel_data_) {
        delete pixel_data_;
    }

    int bufferSize = render_width_ * render_height_;
    pixel_data_ = new UINT32[static_cast<size_t>(bufferSize)];
}
void Renderer::InitializeBitmapinfo() {
    bitmapinfoheader_.biSize = sizeof(BITMAPINFOHEADER);
    bitmapinfoheader_.biWidth = render_width_;
    bitmapinfoheader_.biHeight = -1 * render_height_;
    bitmapinfoheader_.biPlanes = 1;
    bitmapinfoheader_.biBitCount = 32;
    bitmapinfoheader_.biCompression = BI_RGB;
    bitmapinfoheader_.biSizeImage = 0;
    bitmapinfoheader_.biXPelsPerMeter = 0;
    bitmapinfoheader_.biYPelsPerMeter = 0;
    bitmapinfoheader_.biClrUsed = 0;
    bitmapinfoheader_.biClrImportant = 0;

    bitmapinfo_.bmiHeader = bitmapinfoheader_;
}

void Renderer::InitializeBackbuffer() {
    if (window_handle_ == NULL) {
        throw std::exception("failed to create backbuffer: window handle is null");
    }

    if (hdc_backbuffer_) {
        ReleaseDC(window_handle_, hdc_backbuffer_);
    }

    RECT rect;
    GetClientRect(window_handle_, &rect);

    hdc_backbuffer_ = CreateCompatibleDC(hdc_);
    backbuffer_ = CreateCompatibleBitmap(hdc_, rect.right - rect.left, rect.bottom - rect.top);

    SelectObject(hdc_backbuffer_, backbuffer_);

    solid_brush_ = CreateSolidBrush(RGB(255, 0, 0));

    SelectObject(hdc_backbuffer_, solid_brush_);
}

void Renderer::SetWindowResolution(JobbaResolution new_resolution) {
    window_resolution_ = new_resolution;
    GetJobbaResolutionValue(window_resolution_, &window_width_, &window_height_);

    InitializeBackbuffer();
}

void Renderer::SetVirtualResolution() {
    SetVirtualResolution(Settings::current_resolution);
}

void Renderer::SetVirtualResolution(JobbaResolution new_resolution) {
    virtual_resolution_ = new_resolution;
    GetJobbaResolutionValue(virtual_resolution_, &render_width_, &render_height_);


    InitializeBitmapinfo();
    InitializeDataBuffer();
}

void Renderer::SetVideoMode(JobbaVideoMode new_video_mode) {
    video_mode_ = new_video_mode;

    if (video_mode_ == JobbaVideoMode::kWindowedFullscreen) {
        HDC hdc = GetWindowDC(NULL);
        window_width_ = GetDeviceCaps(hdc, HORZRES);
        window_height_ = GetDeviceCaps(hdc, VERTRES);

        InitializeBackbuffer();
    }
    else if (video_mode_ == JobbaVideoMode::kWindowed) {
        SetWindowResolution(Settings::current_resolution);
    }
}

JobbaResolution Renderer::get_virtual_resolution() {
    return virtual_resolution_;
}

JobbaResolution Renderer::get_window_resolution() {
    return window_resolution_;
}

JobbaVideoMode Renderer::get_video_mode() {
    return video_mode_;
}