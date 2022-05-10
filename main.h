#include <windows.h>
#include <time.h>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void DrawPixels(HWND hwnd);