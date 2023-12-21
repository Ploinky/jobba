#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <Windows.h>

#include "renderer.h"
#include <string>
#include "version.h"
#include "jobba_window.h"

static Renderer renderer{};

int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int cmd_show) {
#ifdef _DEBUG
    // Allocate a console for debugging
    AllocConsole();

    // Reroute standard input and output to console
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
#endif

    // TODO: Load settings
    Settings::current_resolution = kQnhd;
    Settings::current_virtual_resolution = kQnhd;
    Settings::current_video_mode = kWindowed;

    JobbaWindow window(Settings::current_resolution);
    window.Show(cmd_show);

    int width;
    int height;
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
        // First check if we have the correct video because...
        if (window.get_current_video_mode() != Settings::current_video_mode) {
            window.SetVideoMode(Settings::current_video_mode);
        }

        // We skip this if we're fullscreen windowed!
        if (window.get_current_video_mode() == JobbaVideoMode::kWindowed && window.get_current_resolution() != Settings::current_resolution) {
            window.SetResolution(Settings::current_resolution);
        }

        // ------------ Rendering system ------------ //
        if (renderer.get_video_mode() != Settings::current_video_mode) {
            renderer.SetVideoMode(Settings::current_video_mode);
        }
        
        if (renderer.get_virtual_resolution() != Settings::current_virtual_resolution) {
            renderer.SetVirtualResolution(Settings::current_virtual_resolution);
        }

        if (window.get_current_video_mode() == JobbaVideoMode::kWindowed && renderer.get_window_resolution() != Settings::current_resolution) {
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