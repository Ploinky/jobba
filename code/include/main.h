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


int clientWidth;
int clientHeight;

typedef struct {
    float x;
    float y;
} vec2_t; 

typedef struct {
    vec2_t start;
    vec2_t end;
} wall_t;

typedef struct {
    wall_t* walls;
    int wallCount;
    float ceilHeight;
    float floorHeight;
} sector_t;

vec2_t playerPos;
float playerA;

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif