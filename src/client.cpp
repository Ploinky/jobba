// Platform independant implementation of the client class

#include "client.hpp"
#include <iostream>
#include "window.hpp"
#include "model3d.hpp"
#include "vertex.hpp"
#include "logger.hpp"
#include "direct3d.hpp"

namespace P3D {
    void Client::Run() {
        Logger::Msg("Hello from general client!");

        // Create and show window
        Window* window = new Window();
        window->Show();

        // ----- Initialize Direct3D -----
        // Must be done after window is showing, otherwise swap chain creation fails
        direct3D = new Direct3D();

        // Immediately close window if initialization of Direct3D fails
        if (!direct3D->Initialize(window->GetWindowHandle())) {
            Logger::Err("Direct3D initialization failed, P3D will quit.");
            window->SetShouldClose();
        }

        Model3D* model = new Model3D();
        Vertex vert[] = { Vertex{{-0.5, -0.5, 0}, {255, 0, 0, 255}},
            Vertex{{0, 0.5, 0}, {0, 255, 0, 255}},
            Vertex{{0.5, -0.5, 0}, {0, 0, 255, 255}}
        };
        model->vertices = vert;
        model->vertexCount = 3;
        
        // Main game loop
        // Keep running while both the client wants to keep runnning and the window has not been closed
        isRunning = true;
        while(isRunning && !window->ShouldClose()) {
            window->HandleEvents();
            BeginRender();
            Render(model);
            FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }
    

    void Client::BeginRender() {
        direct3D->ClearScreen();
    }

    void Client::Render(Model3D* model) {
        direct3D->Render(model);
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

}