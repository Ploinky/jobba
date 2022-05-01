#include <DirectXMath.h>

namespace P3D {
    class Camera {
    public:
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 rotation;
        float fov = 55.0f;
        float nearClip = 0.001f;
        float farClip = 1000.0f;
    };
}