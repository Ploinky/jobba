#pragma once

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _USE_MATH_DEFINES // for C

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

#include <windows.h>
#include <time.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    double x;
    double y;
} vec2_t;

vec2_t subtract(vec2_t a, vec2_t b);

double cross(vec2_t a, vec2_t b);

typedef struct {
    vec2_t tl;
    vec2_t br;
} rect_t;

extern vec2_t g_playerPos;
extern double g_playerA;


double toDegrees(double radians);
double toRadians(double degrees);

int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev_instance, _In_ PWSTR cmd_line, _In_ int cmd_show);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


extern bool* g_keys;