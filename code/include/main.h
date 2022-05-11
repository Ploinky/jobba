#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _USE_MATH_DEFINES // for C

#include <Windows.h>
#include <time.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <math.h>
#include <stdlib.h>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")


typedef uint32_t u32;

u32* memory;
int client_width;
int client_height;

float playerX;
float playerY;

typedef struct {
    float startX;
    float startY;
    float endX;
    float endY;
} wall_t;

float playerA;

typedef struct {
    float x;
    float y;
} vec2_t;


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void draw_pixel(int x, int y, u32 color);
void clear_screen(u32 color);
float cross(vec2_t a, vec2_t b);
vec2_t subtract(vec2_t a, vec2_t b);
float length(vec2_t vec);
float toDegrees(float radians);
float toRadians(float degrees);
LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
