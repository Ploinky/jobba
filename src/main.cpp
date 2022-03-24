// Main entry point for Windows systems

#include <stdio.h>
#include "client.hpp"
#include "logger.hpp"
#include <Windows.h>

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pCmdLine, _In_ int nCmdShow) {
    AllocConsole();

    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    
    P3D::Logger::Msg("Starting P3D Engine!");

    P3D::Client* client = new P3D::Client();
    client->Run();

    P3D::Logger::Msg("Stopping P3D Engine!\n");
    return 0;
}