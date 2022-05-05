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
    Client::~Client() {
        for(Model3D* m : models) {
            delete m;
        }

        models.clear();

        delete renderer;
        renderer = 0;

        delete direct3D;
        direct3D = 0;

        delete window;
        window = 0;

        delete keyboardInput;
        keyboardInput = 0;

        delete mouseInput;
        mouseInput = 0;
    }

    void Client::Run() {
        Logger::Msg("Hello from general client!");

        Model3D* model = new Model3D();
        Vertex vert[] = { Vertex{{-0.5f, 0, 0}, {1.0f, 0, 0, 1}},
            Vertex{{0, 1, 0}, {0, 1.0f, 0, 1}},
            Vertex{{0.5f, 0, 0}, {0, 0, 1.0f, 1}}
        };
        model->vertices = vert;
        model->vertexCount = 3;
        models.push_back(model);

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
        model2->position.y = -0.1f;
        models.push_back(model2);

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
        models.push_back(model3);

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

        mouseInput = new MouseInput();
        keyboardInput = new KeyboardInput();
        
        window->windowResizedHandler = [this]() {
            direct3D->SetWindowDimensions(window->width, window->height);
            renderer->SetAspectRatio((float) window->width / (float) window->height);
        };

        window->keyHandler = [this](long key, bool down) {
            keyboardInput->SetKeyDown((char) key, down);
        };

        window->mouseHandler = [this](short x, short y) {
            mouseInput->SetMousePos(x, y);
        };

        renderer = new Renderer();
        renderer->Initialize(direct3D);
        renderer->camera->position.y = 1;
        renderer->camera->position.z = 0;
        
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

            for(Model3D* m : models) {
                Render(m);
            }

            FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }


    short lastX;

    short lastY;
    bool initialValue = true;


    void Client::HandlePlayerInput(Model3D* model, float dt) {
        if(initialValue) {
            lastX = mouseInput->GetMouseX();
            lastY = mouseInput->GetMouseY();
            initialValue = false;
        }

        short newX = mouseInput->GetMouseX();
        short newY = mouseInput->GetMouseY();
        short mouseX = newX - lastX;
        short mouseY = newY - lastY;
        lastX = newX;
        lastY = newY;

        renderer->camera->rotation.y += ((float) mouseX) * dt * 100.0f;
        renderer->camera->rotation.x += ((float) mouseY) * dt * 100.0f;

        if(keyboardInput->IsKeyDown('D')) {
            renderer->camera->position.x += 10 * dt;
        }

        if(keyboardInput->IsKeyDown('A')) {
            renderer->camera->position.x -= 10 * dt;
        }

        if(keyboardInput->IsKeyDown('W')) {
            renderer->camera->position.z += 10 * dt;
        }

        if(keyboardInput->IsKeyDown('S')) {
            renderer->camera->position.z -= 10 * dt;
        }

        if(keyboardInput->IsKeyDown(VK_ESCAPE)) {
            isRunning = false;
        }

        if(keyboardInput->IsKeyDown(VK_SPACE)) {
            renderer->camera->position.y += 10 * dt;
        }

        if(keyboardInput->IsKeyDown(VK_CONTROL)) {
            renderer->camera->position.y -= 10 * dt;
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