#include "camera.hpp"

namespace P3D {
    Camera::Camera() {
        this->fov = 55;
        this->nearClip = 0.001f;
        this->farClip = 1000.0f;
    }
}