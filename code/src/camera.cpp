#include "camera.hpp"

namespace P3D {
    Camera::Camera() {
        this->fov = 75;
        this->nearClip = 0.001f;
        this->farClip = 1000.0f;
        this->position = DirectX::XMFLOAT3();
        this->rotation = DirectX::XMFLOAT3();
    }
}