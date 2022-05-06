#include "client.hpp"
#include <iostream>
#include "window.hpp"
#include "mesh.hpp"
#include "vertex.hpp"
#include "logger.hpp"
#include "direct3d.hpp"
#include "renderer.hpp"
#include <DirectXMath.h>
#include "camera.hpp"
#include "keyboard_input.hpp"
#include <chrono>

namespace P3D {
    Client::~Client() {
        for(Mesh* m : models) {
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
    }

    void Client::Run() {
        Logger::Msg("Hello from general client!");

        Mesh* model = new Mesh();
        Vertex vert[] = { Vertex{{-0.5f, 0, 0}, {1.0f, 0, 0, 1}},
            Vertex{{0, 1, 0}, {0, 1.0f, 0, 1}},
            Vertex{{0.5f, 0, 0}, {0, 0, 1.0f, 1}}
        };
        model->vertices = vert;
        model->vertexCount = 3;
        models.push_back(model);

        Mesh* model2 = new Mesh();
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

        keyboardInput = new KeyboardInput();
        
        window->windowResizedHandler = [this]() {
            direct3D->SetWindowDimensions(window->width, window->height);
            renderer->SetAspectRatio((float) window->width / (float) window->height);
        };

        window->keyHandler = [this](long key, bool down) {
            keyboardInput->SetKeyDown((char) key, down);
        };

        window->mouseHandler = [this](short x, short y) {
            renderer->camera->rotation.y += x * 0.1f;
            renderer->camera->rotation.x += y * 0.1f;
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

            // Render scene
            BeginRender();

            for(Mesh* m : models) {
                Render(m);
            }

            FinishRender();
        }

        Logger::Msg("Game loop has been stopped.");
    }

    void Client::HandlePlayerInput(Mesh* model, float dt) {
        
        DirectX::XMFLOAT3 move = DirectX::XMFLOAT3(
            keyboardInput->IsKeyDown('D') - keyboardInput->IsKeyDown('A'),
            0,
            keyboardInput->IsKeyDown('W') - keyboardInput->IsKeyDown('S'));
        DirectX::XMFLOAT3 r = renderer->camera->rotation;

        DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationRollPitchYaw(
            DirectX::XMConvertToRadians(r.x),
            DirectX::XMConvertToRadians(r.y),
            DirectX::XMConvertToRadians(r.z));

        DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&move);
        vec = DirectX::XMVector3Transform(vec, rotMat);

        DirectX::XMStoreFloat3(&move, vec);
        
        renderer->camera->position.x += move.x * 10 * dt;
        renderer->camera->position.y += move.y * 10 * dt;
        renderer->camera->position.z += move.z * 10 * dt;

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

    void Client::Render(Mesh* model) {
        renderer->Render(model);
    }

    void Client::FinishRender() {
        direct3D->Present();
    }

    long long Client::GetSystemTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}