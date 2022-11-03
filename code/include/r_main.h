#pragma once

#include "main.h"

//
// Main renderer
//

#define RENDER_MAP_STATIC 0
#define RENDER_MAP_DYNAMIC 1
#define RENDER_MAP_PERSPECTIVE 2

extern int g_mapRenderMode;
extern rect_t g_renderTarget;

extern int g_clientWidth;
extern int g_clientHeight;

extern int g_windowWidth;
extern int g_windowHeight;

extern float g_fovH;

void R_Initialize(HWND window);
void R_ClearScreen(uint32_t color);
void R_SetDrawClip(rect_t clip);
void R_SwapBuffer();

void renderMap();

extern vec2_t drawClipTL;
extern vec2_t drawClipBR;

void setDrawClip(int x1, int y1, int x2, int y2);
void setPixel(int x, int y, uint32_t color);
void clearBuffer(uint32_t color);
void drawRect(int x1, int y1, int x2, int y2, uint32_t color);
void fillRect(int x1, int y1, int x2, int y2, uint32_t color);
void drawLine(int x1, int y1, int x2, int y2, uint32_t color);