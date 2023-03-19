#pragma once

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <Windows.h>

#include <cstdlib>

class Renderer {
public:
	~Renderer();

	void Initialize(HWND hwnd, unsigned int width, unsigned int height);

	void RenderMainMenu();
	void RenderMap();
	void SetPixel(unsigned int x, unsigned int y, UINT32 color);

	void Resize(unsigned int newWidth, unsigned int newHeight);

	void FlipBackBuffer();

private:
	HWND window_handle_;
	HDC hdc_;
	HDC hdc_backbuffer_;
	unsigned int window_width_;
	unsigned int window_height_;
	unsigned int render_width_;
	unsigned int render_height_;
	UINT32* pixel_data_;
	BITMAPINFOHEADER bitmapinfoheader_;
	BITMAPINFO bitmapinfo_;
	HBITMAP backbuffer_;
	HBRUSH solid_brush_;

	void InitializeBitmapinfo();
	void InitializeDataBuffer();
	void InitializeBackbuffer();
};