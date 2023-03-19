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

void Renderer::RenderMap() {
    for (unsigned int x = 0; x < render_width_; x++) {
        for (unsigned int y = 0; y < render_height_; y++) {
            UINT32 color = 0x00FF00;
            if (x > 300 || y > 180) {
                color = 0x0000FF;
            }

            if (x == 319 || y == 199) {
                color = 0xFF00FF;
            }

            SetPixel(x, y, color);
        }
    }

    unsigned int start_x, end_x, start_y, end_y;
    double ratio_w = (double) window_width_ / (double) render_width_;
    double ratio_h = (double) window_height_ / (double) render_height_;
    double ratio = min(ratio_w, ratio_h);

    end_x = render_width_ * ratio;
    end_y = render_height_ * ratio;

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
    bitmapinfoheader_.biHeight = -render_height_;
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

void Renderer::Resize(unsigned int newWidth, unsigned int newHeight) {
    window_width_ = newWidth;
    window_height_ = newHeight;

    InitializeBitmapinfo();
    InitializeBackbuffer();
}
void Renderer::SetVirtualResolution() {
    SetVirtualResolution(window_width_, window_height_);
}

void Renderer::SetVirtualResolution(unsigned int newWidth, unsigned int newHeight) {
    render_width_ = newWidth;
    render_height_ = newHeight;

    InitializeDataBuffer();
}