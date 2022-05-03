#pragma once
#include <DirectXMath.h>

namespace P3D {
    class Camera {
    public:
        Camera();
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 rotation;
        float fov;
        float nearClip;
        float farClip;
    };
}