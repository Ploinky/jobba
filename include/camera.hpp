#include <DirectXMath.h>

namespace P3D {
    class Camera {
    public:
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 rotation;
        float fov = 70;
        float nearClip = 0.001;
        float farClip = 1000;
    };
}