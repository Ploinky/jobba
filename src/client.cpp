// Platform independant implementation of the client class

#include "client.hpp"
#include <iostream>
#include "window.hpp"
#include "model3d.hpp"
#include "vertex.hpp"
#include "logger.hpp"

namespace P3D {
    void Client::Run() {
        Logger::Msg("Hello from general client!");

        // Create and show window
        Window* window = new Window();
        window->Show();

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
            window->BeginRender();
            window->Render(model);
            window->FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }
}