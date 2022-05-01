// Platform independant implementation of the client class

#include "client.hpp"
#include <iostream>
#include "window.hpp"
#include "model3d.hpp"
#include "vertex.hpp"
#include "logger.hpp"
#include "direct3d.hpp"
#include "renderer.hpp"
#include <DirectXMath.h>
#include "camera.hpp"
#include "keyboard_input.hpp"
#include "mouse_input.hpp"

namespace P3D {
    void Client::Run() {
        Logger::Msg("Hello from general client!");

        Model3D* model = new Model3D();
        Vertex vert[] = { Vertex{{-1, -1, 0}, {1.0f, 0, 0, 1}},
            Vertex{{0, 1, 0}, {0, 1.0f, 0, 1}},
            Vertex{{1, -1, 0}, {0, 0, 1.0f, 1}}
        };
        model->vertices = vert;
        model->vertexCount = 3;

        // Create and show window
        window = new Window();
        window->Show();
        
        // ----- Initialize Direct3D -----
        // Must be done after window is showing, otherwise swap chain creation fails
        direct3D = new Direct3D();

        // Immediately close window if initialization of Direct3D fails
        if (!direct3D->Initialize(window->GetWindowHandle())) {
            Logger::Err("Direct3D initialization failed, P3D will quit.");
            window->SetShouldClose();
        }

        window->callback = [this]() {
            direct3D->SetWindowDimensions(window->width, window->height);
            renderer->SetAspectRatio((float) window->width / (float) window->height);
        };

        keyboardInput = new KeyboardInput();

        window->keyEvent = [this](long key, bool down) {
            keyboardInput->SetKeyDown((char) key, down);
        };

        mouseInput = new MouseInput();

        window->mouseEvent = [this](short x, short y) {
            mouseInput->SetMousePos(x, y);
        };

        renderer = new Renderer();
        renderer->Initialize(direct3D);
        renderer->camera->position.y = 10;
        renderer->camera->position.z = -5;
        renderer->camera->rotation.x = 45;
        
        // Main game loop
        // Keep running while both the client wants to keep runnning and the window has not been closed
        isRunning = true;
        while(isRunning && !window->ShouldClose()) {
            // Event handling
            window->HandleEvents();

            // Game logic
            HandlePlayerInput(model);

            // Render scene
            BeginRender();
            Render(model);
            FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }

    void Client::HandlePlayerInput(Model3D* model) {
        if(mouseInput->GetMouseX() <= 0) {
            renderer->camera->position.x -= 0.01;
        }

        if(mouseInput->GetMouseX() >= window->width - 1) {
            renderer->camera->position.x += 0.01;
        }

        if(mouseInput->GetMouseY() <= 0) {
            renderer->camera->position.z += 0.01;
        }

        if(mouseInput->GetMouseY() >= window->height - 1) {
            renderer->camera->position.z -= 0.01;
        }

        if(keyboardInput->IsKeyDown(VK_RIGHT)) {
            renderer->camera->position.x += 0.01;
        }
        if(keyboardInput->IsKeyDown(VK_LEFT)) {
            renderer->camera->position.x -= 0.01;
        }
        if(keyboardInput->IsKeyDown(VK_UP)) {
            renderer->camera->position.z += 0.01;
        }
        if(keyboardInput->IsKeyDown(VK_DOWN)) {
            renderer->camera->position.z -= 0.01;
        }
        if(keyboardInput->IsKeyDown(VK_ESCAPE)) {
            isRunning = false;
        }
    }
    
    void Client::BeginRender() {
        direct3D->ClearScreen();
        renderer->UpdateCameraMatrix();
    }

    void Client::Render(Model3D* model) {
        renderer->Render(model);
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

}