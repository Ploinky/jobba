#ifndef __MAIN_H_
#define __MAIN_H_

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

struct vec2_t {
    float x;
    float y;
};

vec2_t subtract(vec2_t a, vec2_t b) {
    vec2_t ret = { a.x - b.x, a.y - b.y};
    return ret;
}

typedef struct {
    vec2_t tl;
    vec2_t br;
} rect_t;

extern vec2_t g_playerPos;
extern float g_playerA;


float toDegrees(float radians);
float toRadians(float degrees);

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif