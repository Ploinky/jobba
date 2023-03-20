#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <Windows.h>

#include "renderer.h"
#include <string>
#include "version.h"
#include "jobba_window.h"

static Renderer renderer{};

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int cmd_show) {
    // TODO: Load settings
    Settings::current_resolution = kQnhd;
    Settings::current_virtual_resolution = kQnhd;

    JobbaWindow window(Settings::current_resolution);
    window.Show(cmd_show);

    unsigned int width;
    unsigned int height;
    GetJobbaResolutionValue(Settings::current_virtual_resolution, &width, &height);
    renderer.Initialize(window.get_window_handle(), width, height);

    bool is_running = true;

    while (is_running) {
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                is_running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // ------------ Window system ------------ //
        if (window.get_current_resolution() != Settings::current_resolution) {
            window.SetResolution(Settings::current_resolution);
        }

        // ------------ Rendering system ------------ //
        if (renderer.get_virtual_resolution() != Settings::current_virtual_resolution) {
            renderer.SetVirtualResolution(Settings::current_virtual_resolution);
        }

        if (renderer.get_window_resolution() != Settings::current_resolution) {
            renderer.SetWindowResolution(Settings::current_resolution);
        }

        renderer.RenderMap();
        renderer.RenderMainMenu();

#ifdef _DEBUG
        renderer.RenderDebugInfo();
#endif

        renderer.FlipBackBuffer();
    }

    return EXIT_SUCCESS;
}