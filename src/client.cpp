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

namespace P3D {
    void Client::Run() {
        Logger::Msg("Hello from general client!");

        Model3D* model = new Model3D();
        Vertex vert[] = { Vertex{{-10, -10, 0}, {1.0f, 0, 0, 1}},
            Vertex{{0, 10, 0}, {0, 1.0f, 0, 1}},
            Vertex{{10, -10, 0}, {0, 0, 1.0f, 1}}
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

        renderer = new Renderer();
        renderer->Initialize(direct3D);
        
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
            DirectX::XMFLOAT3 position = renderer->camera->position;
            DirectX::XMFLOAT3 rotation = renderer->camera->rotation;

            if(keyboardInput->IsKeyDown('W')) {
                position.z += 0.01;
            }

            if(keyboardInput->IsKeyDown('S')) {
                position.z -= 0.01;
            }

            if(keyboardInput->IsKeyDown('A')) {
                position.x -= 0.01;
            }

            if(keyboardInput->IsKeyDown('D')) {
                position.x += 0.01;
            }

            if(keyboardInput->IsKeyDown('Q')) {
                rotation.y -= 0.01;
            }

            if(keyboardInput->IsKeyDown('E')) {
                rotation.y += 0.01;
            }

            renderer->SetCameraPosition(position);
            renderer->SetCameraRotation(rotation);

            if(keyboardInput->IsKeyDown(VK_RIGHT)) {
                model->position.x += 0.01;
            }
            if(keyboardInput->IsKeyDown(VK_LEFT)) {
                model->position.x -= 0.01;
            }
            if(keyboardInput->IsKeyDown(VK_UP)) {
                model->position.z += 0.01;
            }
            if(keyboardInput->IsKeyDown(VK_DOWN)) {
                model->position.z -= 0.01;
            }
            if(keyboardInput->IsKeyDown(VK_ESCAPE)) {
                isRunning = false;
            }
    }
    

    void Client::BeginRender() {
        direct3D->ClearScreen();
    }

    void Client::Render(Model3D* model) {
        renderer->Render(model);
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

}