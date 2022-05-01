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
#include <chrono>

namespace P3D {
    void Client::Run() {
        Logger::Msg("Hello from general client!");

        Model3D* model = new Model3D();
        Vertex vert[] = { Vertex{{-0.5f, 0, 0}, {1.0f, 0, 0, 1}},
            Vertex{{0, 1, 0}, {0, 1.0f, 0, 1}},
            Vertex{{0.5f, 0, 0}, {0, 0, 1.0f, 1}}
        };
        model->vertices = vert;
        model->vertexCount = 3;

        Model3D* model2 = new Model3D();
        Vertex vert2[] = {
            Vertex{{-10, 0, -10}, {1.0f, 1.0f, 1.0f, 1}},
            Vertex{{-10, 0, 10}, {1.0f, 0, 0, 1}},
            Vertex{{10, 0, -10}, {0, 1.0f, 0, 1}},
            Vertex{{-10, 0, 10}, {1.0f, 0, 0, 1}},
            Vertex{{10, 0, 10}, {0, 0, 1.0f, 1}},
            Vertex{{10, 0, -10}, {0, 1.0f, 0, 1}},
        };
        model2->vertices = vert2;
        model2->vertexCount = 6;

        Model3D* model3 = new Model3D();
        Vertex vert3[] = {
            Vertex{{-1, 0, 0}, {1.0f, 1.0f, 1.0f, 1}},
            Vertex{{-1, 0.3f, 0}, {1.0f, 1.0f, 1.0f, 1}},
            Vertex{{1, 0.3f, 0}, {1.0f, 1.0f, 1.0f, 1}},
            Vertex{{-1, 0, 0}, {1.0f, 1.0f, 1.0f, 1}},
            Vertex{{1, 0.3f, 0}, {1.0f, 1.0f, 1.0f, 1}},
            Vertex{{1, 0, 0}, {1.0f, 1.0f, 1.0f, 1}},
        };
        model3->position = model->position;
        model3->rotation.x = 60;
        model3->vertices = vert3;
        model3->vertexCount = 6;

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
        renderer->camera->position.y = 20;
        renderer->camera->position.z = -12;
        renderer->camera->rotation.x = 60;
        
        // Main game loop
        // Keep running while both the client wants to keep runnning and the window has not been closed
        isRunning = true;
        lastFrame = GetSystemTime();
        while(isRunning && !window->ShouldClose()) {
            auto thisFrame = GetSystemTime();
            float dt = (thisFrame - lastFrame) / 1000000.0f / 1000.0f;
            lastFrame = thisFrame;

            // Event handling
            window->HandleEvents();

            // Game logic
            HandlePlayerInput(model, dt);
            
            model3->position = model->position;
            model3->position.y = model->position.y + 1.4f;

            // Render scene
            BeginRender();
            Render(model);
            Render(model2);
            Render(model3);
        
            FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }

    void Client::HandlePlayerInput(Model3D* model, float dt) {
        if(mouseInput->GetMouseX() <= 0) {
            renderer->camera->position.x -= 10 * dt;
        }

        if(mouseInput->GetMouseX() >= window->width - 1) {
            renderer->camera->position.x += 10 * dt;
        }

        if(mouseInput->GetMouseY() <= 0) {
            renderer->camera->position.z += 10 * dt;
        }

        if(mouseInput->GetMouseY() >= window->height - 1) {
            renderer->camera->position.z -= 10 * dt;
        }

        if(keyboardInput->IsKeyDown(VK_RIGHT)) {
            renderer->camera->position.x += 10 * dt;
        }

        if(keyboardInput->IsKeyDown(VK_LEFT)) {
            renderer->camera->position.x -= 10 * dt;
        }

        if(keyboardInput->IsKeyDown(VK_UP)) {
            renderer->camera->position.z += 10 * dt;
        }

        if(keyboardInput->IsKeyDown(VK_DOWN)) {
            renderer->camera->position.z -= 10 * dt;
        }

        if(keyboardInput->IsKeyDown('D')) {
            model->position.x += 10 * dt;
        }

        if(keyboardInput->IsKeyDown('A')) {
            model->position.x -= 10 * dt;
        }

        if(keyboardInput->IsKeyDown('W')) {
            model->position.z += 10 * dt;
        }

        if(keyboardInput->IsKeyDown('S')) {
            model->position.z -= 10 * dt;
        }

        if(keyboardInput->IsKeyDown(VK_ESCAPE)) {
            isRunning = false;
        }

        if(keyboardInput->IsKeyDown(VK_SPACE)) {
            renderer->camera->position.x = model->position.x;
            renderer->camera->position.z = model->position.z - 12;
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

    long long Client::GetSystemTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}