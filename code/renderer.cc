#include "renderer.hpp"
#include <exception>

Renderer::~Renderer() {
}
void Renderer::Initialize(HWND hwnd, unsigned int width, unsigned int height) {
    window_width_ = width;
    window_height_ = height;

    render_width_ = 320;
    render_height_ = 320;

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

    StretchDIBits(hdc_backbuffer_, 0, 0, window_width_, window_height_, 0, 0, render_width_, render_height_, pixel_data_, &bitmapinfo_, DIB_RGB_COLORS, SRCCOPY);
}

void Renderer::RenderMainMenu() {
    RECT rect{ 0, 0, 100, 100 };
    FillRect(hdc_backbuffer_, &rect, solid_brush_);
    SetBkMode(hdc_backbuffer_, TRANSPARENT);
    DrawText(hdc_backbuffer_, L"test123", -1, &rect, NULL);
}

void Renderer::FlipBackBuffer() {
    RECT rc;
    GetClientRect(window_handle_, &rc);

    BitBlt(hdc_,
        rc.left, rc.top,
        rc.right - rc.left, rc.bottom - rc.top,
        hdc_backbuffer_,
        0, 0,
        SRCCOPY);
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